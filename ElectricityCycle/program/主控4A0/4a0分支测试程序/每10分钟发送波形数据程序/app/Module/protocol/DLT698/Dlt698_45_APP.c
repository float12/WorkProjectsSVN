//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部
//创建人	姜文浩 魏灵坤
//创建日期	2016.8.4
//描述		DL/T 698.45面向对象协议C文件
//修改记录
//----------------------------------------------------------------------
#include "AllHead.h"
#include "Dlt698_45.h"
#include "GDW698/GDW698.h"
#include "MCollect.h"
#include "bsp_cpu_flash.h"
#if ( SEL_DLT698_2016_FUNC == YES  )

//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
// 索引信息在EEPROM中的地址范围


//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------
//-----------------------------------------------
//				全局使用的变量，常量
__no_init BYTE g_PassWordErrCounter;//密码错误次数
BYTE RN[MAX_RN_SIZE];
BYTE g_TimeTagBuf[12];
BYTE g_byClrTimer_500ms;
BYTE ClientAddress;//客户机地址
BYTE ConnectMode;  //连接模式---只用于上位机读取连接机制-无其他作用
TGetRequestNext pGetRequestNext[MAX_COM_CHANNEL_NUM];




TDLT698LinkData LinkData[MAX_COM_CHANNEL_NUM];
TDLT698APPData APPData[MAX_COM_CHANNEL_NUM];
BYTE DLT698APDURequestBuf[MAX_APDU_SIZE+EXTRA_BUF];
BYTE ProtocolBuf[MAX_APDU_SIZE+EXTRA_BUF];// MAX_APDU_SIZE = 2000, EXTRA_BUF = 30
BYTE ProtocolBufBack[MAX_APDU_SIZE+EXTRA_BUF];// MAX_APDU_SIZE = 2000, EXTRA_BUF = 30
__no_init BYTE SecurityRequestBuf[50+EXTRA_BUF];//下发解密数据存放
__no_init BYTE SecurityResponseBuf[50+EXTRA_BUF];//返回解密数据存放

TDLT698APPConnect APPConnect;
TSafeModePara SafeModePara;
TAPDUBufFlag APDUBufFlag;
TConsultData ConsultData;
T_Iap tIap;
BYTE IapStatus[(MAX_SIZE_FOR_IAP/LEN_OF_ONE_FRAME)/8+1];// F001 的属性4 升级状态；后续改为按照实际一帧传输的长度开辟空间!!!!!!
TIapInfo tIapInfo;// 临时使用，不需保存
//-----------------------------------------------

//-----------------------------------------------
//				本文件使用的变量，常量
const DWORD TIMinutes[] = { 1,1,60,1440,43200,525600 };//时间间隔 秒 分 时 日 不处理月 秒特殊处理
const DWORD FollowReportOAD[] = { 0x201e0200, 0x20200200 };//跟随上报需要读取记录的内容 事件发生时间 事件结束时间
const DWORD FixedFreezeOAD[] = { 2,0x00022320, 0x00022120 };//OAD个数  冻结记录序号OAD  数据冻结时间OAD

const DWORD StandardEventUnit[6][2] =		
{ 	//标准事件记录单元
	{0X00000005,0x00000000 },//行数
	{0X01020133,0x00022220 },//事件记录序号
	{0X02020133,0x00021e20 },//事件发生时间
	{0X03020133,0x00022020 },//事件结束时间
	{0X04020133,0x00022420 },//事件发生源
	{0X05020133,0x00020033 },//事件上报状态
}; 
									
const DWORD ProgEventUnit[7][2] =			
{ 	//编程事件记录单元
	{0X00000006,0x00000000 },//行数
	{0X01020233,0x00022220 },//事件记录序号
	{0X02020233,0x00021e20 },//事件发生时间
	{0X03020233,0x00022020 },//事件结束时间
	{0X04020233,0x00022420 },//事件发生源
	{0X05020233,0x00020033 },//事件上报状态
	{0X06020233,0X06020233 },//编程对象列表
} ;
									
const DWORD OverDemandEventUnit[8][2] =		
{ 	//需量超限事件记录单元
	{0X00000007,0x00000000 },//行数
	{0X01020833,0x00022220 },//事件记录序号
	{0X02020833,0x00021e20 },//事件发生时间
	{0X03020833,0x00022020 },//事件结束时间
	{0X04020833,0x00022420 },//事件发生源
	{0X05020833,0x00020033 },//事件上报状态
	{0X06020833,0X06020833 },//超限期间正向有功需量最大值
	{0X07020833,0X07020833 },//超限期间需量最大值发生时间
}; 
										
const DWORD EventClearUnit[7][2] =			
{ 	//事件清零记录单元
	{0X00000006,0x00000000 },//行数
	{0X01020c33,0x00022220 },//事件记录序号
	{0X02020c33,0x00021e20 },//事件发生时间
	{0X03020c33,0x00022020 },//事件结束时间
	{0X04020c33,0x00022420 },//事件发生源
	{0X05020c33,0x00020033 },//事件上报状态
	{0X06020c33,0X06020c33 },//事件清零列表
} ;	
											
const DWORD EventPhaseUnit[8][2] =			
{ 	//分相事件记录单元
	{0X00000004,0x00000000 },//行数
	{0x00022220,0x00022220 },//事件记录序号
	{0x00021e20,0x00021e20 },//事件发生时间
	{0x00022020,0x00022020 },//事件结束时间
	{0x00020033,0x00020033 },//事件上报状态
} ;		

const DWORD EventSegTableUnit[8][2]=
{// 时段表编程记录单元0x3314
	{0X00000007,0x00000000 },//行数
	{0X01021433,0x00022220 },//事件记录序号
	{0X02021433,0x00021e20 },//事件发生时间
	{0X03021433,0x00022020 },//事件结束时间
	{0X04021433,0x00022420 },//事件发生源
	{0X05021433,0x00020033 },//事件上报状态
	{0X06021433,0X06021433 },//编程日时段对象OAD
	{0X07021433,0X07021433 },//编程日时段表
};	

const DWORD EventHolidayUnit[8][2]=
{// 节假日编程记录单元0x3315
	{0X00000007,0x00000000 },//行数
	{0X01021533,0x00022220 },//事件记录序号
	{0X02021533,0x00021e20 },//事件发生时间
	{0X03021533,0x00022020 },//事件结束时间
	{0X04021533,0x00022420 },//事件发生源
	{0X05021533,0x00020033 },//事件上报状态
	{0X06021533,0X06021533 },//编程节假日对象OAD
	{0X07021533,0X07021533 },//编程节假日数据
};	

const DWORD ABR_CARDUnit[10][2] =			
{ 	//异常插卡事件记录单元0x3310
	{0X00000009,0x00000000 },//行数
	{0X01021033,0x00022220 },//事件记录序号double-long-unsigned，
	{0X02021033,0x00021e20 },//事件发生时间 date_time_s，
	{0X03021033,0x00022020 },//事件结束时间date_time_s，
	{0X04021033,0x00022420 },//事件发生源2017年9号补遗:对象“异常插卡事件记录单元”（OI：3310），事件发生源定义为NULL。
	{0X05021033,0x00020033 },//事件上报状态
	{0X06021033,0X06021033 },//卡序列号	octet-string，
	{0X07021033,0X07021033 },//插卡错误信息字	unsigned，
	{0X08021033,0X08021033 },//插卡操作命令头	octet-string，
	{0X09021033,0X09021033 },//插卡错误响应状态  long-unsigned，
} ;	

const DWORD ReturnMoneyUnit[7][2] =			
{ 	//退费事件记录单元0x3311
	{0X00000006,0x00000000 },//行数
	{0X01021033,0x00022220 },//事件记录序号double-long-unsigned，
	{0X02021033,0x00021e20 },//事件发生时间 date_time_s，
	{0X03021033,0x00022020 },//事件结束时间date_time_s，
	{0X04021033,0x00022420 },//事件发生源NULL。
	{0X05021033,0x00020033 },//事件上报状态
	{0X06021033,0X06021133 },//退费金额      double-long-unsigned（单位：元，换算：-2），
} ;	

const DWORD ChargingWARNINGUnit[8][2] =			
{ 	//充电桩事件拓展单元
	{0X00000007,0x00000000 },//行数
	{0X01022A33,0x00022220 },//事件记录序号
	{0X02022A33,0x00021e20 },//事件发生时间
	{0X03022A33,0x00022020 },//事件结束时间
	{0X04022A33,0x00022420 },//事件发生源
	{0X05022A33,0x00020033 },//事件上报状态
	{0X06022A33,0X06022A33 },//附属数据  bit-string
	{0X07022A33,0X07022A33 },//附属数据  bit-string
} ;	
const DWORD ChargingERRUnit[8][2] =			
{ 	//充电桩事件拓展单元
	{0X00000007,0x00000000 },//行数
	{0X01022B33,0x00022220 },//事件记录序号
	{0X02022B33,0x00021e20 },//事件发生时间
	{0X03022B33,0x00022020 },//事件结束时间
	{0X04022B33,0x00022420 },//事件发生源
	{0X05022B33,0x00020033 },//事件上报状态
	{0X06022B33,0X06022B33 },//附属数据  bit-string
	{0X07022B33,0X07022B33 },//附属数据  bit-string
} ;	
const DWORD ChargingCOM_EXCTIONUnit[8][2] =			
{ 	//充电桩事件拓展单元
	{0X00000007,0x00000000 },//行数
	{0X01022C33,0x00022220 },//事件记录序号
	{0X02022C33,0x00021e20 },//事件发生时间
	{0X03022C33,0x00022020 },//事件结束时间
	{0X04022C33,0x00022420 },//事件发生源
	{0X05022C33,0x00020033 },//事件上报状态
	{0X06022C33,0X06022C33 },//附属数据  bit-string
	{0X07022C33,0X07022C33 },//附属数据  bit-string
} ;	
const DWORD ModuleEventUnit[][2]=
{	//拓展模块事件单元
	{0X00000007,0x00000000 },//行数
	{0X0102E033,0x00022220 },//事件记录序号
	{0X0202E033,0x00021e20 },//事件发生时间
	{0X0302E033,0x00022020 },//事件结束时间
	{0X0402E033,0x00022420 },//事件发生源
	{0X0502E033,0x00020033 },//事件上报状态
	{0X0602E033,0X0602E033 },//附属数据  octet-string
	{0X0702E033,0X0702E033 },//附属数据  octet-string
};	

#if (SEL_EVENT_FIND_UNKNOWN_METER == YES )
const DWORD FindUnknownMeterUnit[7][2] =			
{ 	
	{0X00000006,0x00000000 },//行数
	{0X01020333,0x00022220 },//事件记录序号
	{0X02020333,0x00021e20 },//事件发生时间
	{0X03020333,0x00022020 },//事件结束时间
	{0X04020333,0x00022420 },//事件发生源
	{0X05020333,0x00020033 },//事件上报状态
	{0X06020333,0X00020260 },//搜表结果 Array
} ;	
#endif

#if (SEL_TOPOLOGY == YES )
const DWORD TopoUnit[7][2] =			
{ 	
	{0X00000006,0x00000000 },//行数
	{0X0102833E,0x00022220 },//事件记录序号
	{0X0202833E,0x00021e20 },//事件发生时间
	{0X0302833E,0x00022020 },//事件结束时间
	{0X0402833E,0x00022420 },//事件发生源
	{0X0502833E,0x00020033 },//事件上报状态
	{0X0602833E,0X0602833E },//识别结果 Array
} ;
#endif

const DWORD (*pEventUnit[])[2] = 		
{
	EventPhaseUnit,			//3000	电能表失压事件
	EventPhaseUnit,			//3001	电能表欠压事件
	EventPhaseUnit,			//3002	电能表过压事件
	EventPhaseUnit,			//3003	电能表断相事件
	EventPhaseUnit,			//3004	电能表失流事件
	EventPhaseUnit,			//3005	电能表过流事件
	EventPhaseUnit,			//3006	电能表断流事件
	EventPhaseUnit,			//3007	电能表功率反向事件
	EventPhaseUnit,			//3008	电能表过载事件
	OverDemandEventUnit,	//3009	电能表正向有功需量超限事件
	OverDemandEventUnit,	//300A	电能表反向有功需量超限事件
	OverDemandEventUnit,	//300B	电能表无功需量超限事件
	StandardEventUnit,		//300C	
	StandardEventUnit,		//300D	电能表全失压事件
	StandardEventUnit,		//300E	电能表辅助电源掉电事件
	StandardEventUnit,		//300F	电能表电压逆相序事件
	StandardEventUnit,		//3010	电能表电流逆相序事件
	StandardEventUnit,		//3011	电能表掉电事件
	ProgEventUnit,			//3012	电能表编程事件
	StandardEventUnit,		//3013	电能表清零事件
	StandardEventUnit,		//3014	电能表需量清零事件
	EventClearUnit,			//3015	电能表事件清零事件
	StandardEventUnit,		//3016	电能表校时事件
	EventSegTableUnit,		//3017	电能表时段表编程事件
	StandardEventUnit,		//3018	电能表时区表编程事件
	StandardEventUnit,		//3019	电能表周休日编程事件
	StandardEventUnit,		//301A	电能表结算日编程事件
	StandardEventUnit,		//301B	电能表开盖事件
	StandardEventUnit,		//301C	电能表开端钮盒事件
	StandardEventUnit,		//301D	电能表电压不平衡事件
	StandardEventUnit,		//301E	电能表电流不平衡事件
	StandardEventUnit,		//301F	电能表跳闸事件
	StandardEventUnit,		//3020	电能表合闸事件
	EventHolidayUnit,		//3021	电能表节假日编程事件
	StandardEventUnit,		//3022	电能表有功组合方式编程事件
	StandardEventUnit,		//3023	电能表无功组合方式编程事件
	StandardEventUnit,		//3024	电能表费率参数表编程事件
	StandardEventUnit,		//3025	电能表阶梯表编程事件
	StandardEventUnit,		//3026	电能表密钥更新事件
	ABR_CARDUnit,			//3027	异常插卡记录	
	StandardEventUnit,		//3028	电能表购电记录
	ReturnMoneyUnit,		//3029	退费记录		
	StandardEventUnit,		//302A	电能表恒定磁场干扰事件
	StandardEventUnit,		//302B	电能表负荷开关误动作事件
	StandardEventUnit,		//302C	电能表电源异常事件
 	StandardEventUnit,		//302D	电能表电流严重不平衡事件	
 	StandardEventUnit,		//302E	电能表时钟故障事件
 	StandardEventUnit,		//302F	电能表计量芯片故障事件
 	EventPhaseUnit,			//303B	电能表功率因数超下限事件
 	StandardEventUnit,		//303C	电能表广播校时事件
 	StandardEventUnit,		//3040	电能表零线电流异常
 	StandardEventUnit,		//3040	状态量变位事件
};

static const BYTE IapFacCode[16] = {"WisdomWaveRecord"};
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
static void ProcDlt698App( BYTE Ch);
static void DealGet_Request(BYTE Ch,BYTE *pAPDU);
static WORD AnalyseDlt698ApduLen( BYTE Ch);
//-----------------------------------------------
//				函数定义
//-----------------------------------------------
#if (SEL_TOPOLOGY == YES )
eAPPBufResultChoice ActionRequestTopology( BYTE Ch, BYTE *pOMD);
#endif

extern WORD JudgeRecMeterNo_Dlt698_45(TSerial *p);
//--------------------------------------------------
//功能描述:  电表清零函数
//         
//参数:     无
//         
//返回值:    无 
//         
//备注内容:  无
//--------------------------------------------------
static void ClearMeter( void )
{
	api_ClrFreezeData();
	api_ClrEnergy();
	api_ClrAllEvent( eEXCEPT_CLEAR_METER );
	api_ClrPrePay();
	Task_ClearMeter_Relay();
	
	#if( MAX_PHASE == 3 )
	#if( SEL_DEMAND_2022 == NO )//功率需量法
	api_ClrDemand( eClearAllDemand );
	#else
	api_ClrDemand( eClearAllDemand, eDemandPause );
	#endif
	#endif
	
	#if( SEL_AHOUR_FUNC == YES)
	api_ClrAHour();
	#endif
	#if( SEL_STAT_V_RUN == YES )
	api_ClearVRunTimeNow( 0xff );
	#endif
	    
	api_ClrRunHardFlag( eRUN_HARD_CLOCK_BAT_LOW );
	api_ClrRunHardFlag( eRUN_HARD_READ_BAT_LOW );
	api_ClrRunHardFlag( eRUN_HARD_ERR_RTC_FLAG );
	api_ClrRunHardFlag( eRUN_HARD_BROADCAST_ERR_FLAG);
	api_ClrRunHardFlag( eRUN_HARD_ALREADY_BROADCAST_FLAG );
	api_ClrRunHardFlag( eRUN_HARD_ALREADY_PLAINTEXT_BROADCAST_ERR_FLAG );
	g_PassWordErrCounter = 0;
	memset( g_645PassWordErrCounter, 0x00, sizeof(g_645PassWordErrCounter) );
    
    #if (SEL_SEARCH_METER == YES )
    api_ClearSchMeter();
    #endif
}

//--------------------------------------------------
//功能描述:  698.45上电函数
//         
//参数  :   无
//
//返回值:    无  
//         
//备注内容:  无
//--------------------------------------------------
void PowerUpDlt698(void)
{
	BYTE i;
	
	memset( &APPConnect,0x00,sizeof( APPConnect ));//初始化建立应用连接结构体
	g_byClrTimer_500ms = 0;
	FactoryTime = 0;//清零esam打开厂内模式时间
	api_ClrSysStatus(eSYS_STATUS_ID_698MASTER_AUTH);
	api_ClrSysStatus(eSYS_STATUS_ID_698TERMINAL_AUTH);
	api_ClrSysStatus(eSYS_STATUS_ID_698GENERAL_AUTH);

	for( i=0; i<MAX_COM_CHANNEL_NUM; i++ )
	{
		Serial[i].Addr_Len = 6;//通信地址默认6字节
	}
		
	api_VReadSafeMem( GET_SAFE_SPACE_ADDR( ProSafeRom.SafeModePara), sizeof(TSafeModePara), (BYTE*)&SafeModePara );	
	PowerUpInitAPDUBufFlag();
}

//-----------------------------------------------
//功能描述  :   初始化698参数
//
//参数  : 	无
//
//返回值:      无
//
//备注内容  :   无
//-----------------------------------------------
void FactoryInitDLT698(void)
{
	TTimeBroadCastPara tmpTimeBroadCastPara;
	//初始化广播校时参数
	tmpTimeBroadCastPara.TimeMinLimit = TimeBroadCastTimeMinLimit;
	tmpTimeBroadCastPara.TimeMaxLimit = TimeBroadCastTimeMaxLimit;
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( ProSafeRom.TimeBroadCastPara), sizeof(TTimeBroadCastPara), (BYTE*)&tmpTimeBroadCastPara );

    g_PassWordErrCounter = 0;
}

//-----------------------------------------------
//功能描述  :   初始化698标志
//
//参数  : 	无
//
//返回值:      无
//
//备注内容  :   无
//-----------------------------------------------
void InitDLT698Flag( BYTE Ch )
{
    APPData[Ch].APPFlag = 0;
}

//-----------------------------------------------
//功能描述  : 698.45主函数
//
//参数  :    TSerial *p[in]
//
//返回值:     无
//备注内容  :  无
//-----------------------------------------------
void ProcMessageDlt698(TSerial *p)
{
	BYTE i;
	
	for( i=0; i<MAX_COM_CHANNEL_NUM; i++ )
	{
		if( p == &Serial[i] )
		{
			FunctionConst(PROTOCOL698_TASK1);
			Proc698LinkDataRequest(i);		//链路层请求处理函数
			FunctionConst(PROTOCOL698_TASK2);
			ProcDlt698App(i);             	//应用层处理函数
			FunctionConst(PROTOCOL698_TASK3);
			Proc698LinkDataResponse(i);		//链路层响应处理函数
			break;
		}
	}
}
//-----------------------------------------------
//功能描述  : 698.45规约预处理
//
//参数  :    TSerial *p[in]
//
//返回值:     BOOL （TRUE-拦截透明代理报文 或者 采集接收处理）
//备注内容  :  无
//-----------------------------------------------
BOOL Pre_Dlt698(TSerial *p, BYTE *pAck)
{
	BYTE i = 0;
	BOOL bRc = FALSE;
	BYTE IsMyMeteraddr;

	if( JudgeRecMeterNo_Dlt698_45(p) == FALSE )
	{
		IsMyMeteraddr = FALSE;
	}
	else
	{
		IsMyMeteraddr = TRUE;
	}
	
	for( i=0; i<MAX_COM_CHANNEL_NUM; i++ )
	{
		if( p == &Serial[i] )
		{
			if (IsUpCommuFrame(PRO_DLT698_45,Serial[i].ProBuf))
			{
				if ( IsJLing(i) ) /* 级联应答 */
				{		
					JLWriteRx(i,Serial[i].ProBuf,Serial[i].ProStepDlt698_45);
					bRc = TRUE;
				}
				else  /* 抄表应答 */
				{
					GyRxMonitor(i,PRO_DLT698_45);
					bRc = TRUE;
				}
			} 
			else if( ( (MessageTransParaRam.AllowUnknownAddrTrans == TRUE) && (IsMyMeteraddr== FALSE) )
					|| (IsInSysMeters(&p->ProBuf[5]))
				//  && (IsChargeMeters(&p->ProBuf[5]) == FALSE)
			)//采集器模式处理
			{
				CollectorWriteTx(i,Serial[i].ProBuf,&p->ProBuf[5],Serial[i].ProStepDlt698_45);
				bRc = TRUE;
			}

			break;
		}
	}
	return bRc;
}
//--------------------------------------------------
//功能描述:断开应用链接
//         
//参数  :   无
//
//返回值:    无   
//         
//备注内容:  无
//--------------------------------------------------
void api_Release698_Connect( void )
{
	//红外认证时间698和645用同一个变量 此处不能清除红外认证时间 否则红外认证再身份认证 红外认证时间就清零了
	memset( &APPConnect.ConnectInfo, 0x00, sizeof(APPConnect.ConnectInfo) );
	APPConnect.TerminalAuthTime = 0;
	
	api_ClrSysStatus(eSYS_STATUS_ID_698MASTER_AUTH);
	api_ClrSysStatus(eSYS_STATUS_ID_698TERMINAL_AUTH);
	api_ClrSysStatus(eSYS_STATUS_ID_698GENERAL_AUTH);
}
//--------------------------------------------------
//功能描述:时效性任务判断 包括（断开应用链接、红外时效、延时读取）
//         
//参数  :   无
//
//返回值:    无   
//         
//备注内容:  无
//--------------------------------------------------
void TimeLinessEventJudge( void )
{
    BYTE Result,pBuf[20],i;
    TFourByteUnion Time;
	WORD EsamLen;
	eConnectMode ConnectMode;

	for( i = 0; i < eConnectModeMaxNum; i++ )
	{
		if( APPConnect.ConnectInfo[i].ConnectValidTime != 0 )
		{
			if( i == eConnectGeneral )//一般密码进行极限值判断 其他建立应用链接 esam会保证，因此不怕乱
			{
				if( APPConnect.ConnectInfo[i].ConnectValidTime > 604800)//时间超过7天因为数据非法
				{
					APPConnect.ConnectInfo[i].ConnectValidTime = 1;
				}
			}
			
			if( i > eConnectGeneral )//对称加密模式
			{
				if(((APPConnect.ConnectInfo[i].ConnectValidTime%60)==0)
				&&(   (api_GetSysStatus( eSYS_STATUS_ID_698MASTER_AUTH ) == TRUE)
					||(api_GetSysStatus( eSYS_STATUS_ID_698TERMINAL_AUTH ) == TRUE)) )//每分钟进行会话剩余时间判断-如果剩余时间为零结束当前连接
				{
					if( i == eConnectTerminal )
					{
						EsamLen = api_GetEsamInfo( 0x14, pBuf ); //读f1000500 ESAM对象的属性14．会话时效门限
					}
					else
					{
						EsamLen = api_GetEsamInfo( 5, pBuf ); //读f1000500 ESAM对象的属性5．会话时效门限
					}
					if( EsamLen > (sizeof(pBuf) - 2) )
					{
						APPConnect.ConnectInfo[i].ConnectValidTime = 1;
					}
					
					lib_ExchangeData( Time.b, pBuf+4, 4 );

					if( Time.d == 0 )
					{
						APPConnect.ConnectInfo[i].ConnectValidTime = 1;
					}	
				}
			}

			APPConnect.ConnectInfo[i].ConnectValidTime -= 1;

			if( APPConnect.ConnectInfo[i].ConnectValidTime == 0 )
			{
				if( (APPConnect.ConnectInfo[eConnectGeneral].ConnectValidTime == 0)//主站、终端、一般建立应用链接同时为0时会复位esam
				&& (APPConnect.ConnectInfo[eConnectMaster].ConnectValidTime == 0)
				&& (APPConnect.ConnectInfo[eConnectTerminal].ConnectValidTime == 0) )
				{
					if( (api_GetSysStatus( eSYS_STATUS_ID_698MASTER_AUTH ) == TRUE)
					||(api_GetSysStatus( eSYS_STATUS_ID_698TERMINAL_AUTH ) == TRUE) 
					||(api_GetSysStatus( eSYS_STATUS_ID_698GENERAL_AUTH ) == TRUE))
					{
						api_SaveProgramRecord( EVENT_END, 0x51, (BYTE *)NULL);
					}
					else if( api_GetSysStatus( eSYS_STATUS_ID_645AUTH ) == TRUE )
					{
						api_SaveProgramRecord645(EVENT_END, (BYTE *)NULL, (BYTE *)NULL );
					}
					else
					{

					}
					api_ResetEsamSpi();
					ESAMSPIPowerDown( ePowerOnMode );//关闭esam电源
					api_ClrSysStatus(eSYS_STATUS_ID_698MASTER_AUTH);
					api_ClrSysStatus(eSYS_STATUS_ID_698TERMINAL_AUTH);
					api_ClrSysStatus(eSYS_STATUS_ID_698GENERAL_AUTH);
					api_ClrSysStatus(eSYS_STATUS_ID_645AUTH);
					ConsultData.channel = 0X55;//恢复默认值
					ConsultData.Len = MAX_PRO_BUF;
				}	

				if( i == eConnectGeneral )//一般密码时间到
				{
					api_ClrSysStatus(eSYS_STATUS_ID_698GENERAL_AUTH);
				}
				else if( i == eConnectMaster )//主站应用链接时间到
				{
					api_ClrSysStatus(eSYS_STATUS_ID_698MASTER_AUTH);
					api_ClrSysStatus(eSYS_STATUS_ID_645AUTH);
				}
				else//终端应用链接时间到
				{
					api_ClrSysStatus(eSYS_STATUS_ID_698TERMINAL_AUTH);
				}
				
				APPConnect.ConnectInfo[i].ConstConnectValidTime = 0;
			}

		}

	}

	if( APPConnect.IRAuthTime != 0 )
	{
		if( APPConnect.IRAuthTime > 604800 )//时间超过7天 认为时间非法
		{
			APPConnect.IRAuthTime = 1;
		}
		
        APPConnect.IRAuthTime = (APPConnect.IRAuthTime -1);
        if( APPConnect.IRAuthTime == 0 )
        {
            api_ClrSysStatus( eSYS_IR_ALLOW_PRG );//时间到 清红外认证标志
        }
	}
	
	if( APPConnect.TerminalAuthTime != 0 )
	{
		if( APPConnect.TerminalAuthTime > 604800 )//时间超过7天 认为时间非法
		{
			APPConnect.TerminalAuthTime = 1;
		}
		
        APPConnect.TerminalAuthTime = (APPConnect.TerminalAuthTime -1);
        if( APPConnect.TerminalAuthTime == 0 )
        {
            api_ClrSysStatus( eSYS_TERMINAL_ALLOW_PRG );//时间到 清红外认证标志
        }
	}

	if( FactoryTime != 0 )//厂内模式时间进行扣减
	{
        FactoryTime --;
	}

	if( FrameOverTime != 0 )//分帧延时扣减
	{
		FrameOverTime --;
	}
}

//--------------------------------------------------
//功能描述: APDUBufFlag上电初始化
//         
//参数  :    无
//
//返回值:     无   
//         
//备注内容:    无
//--------------------------------------------------
void PowerUpInitAPDUBufFlag( void )
{
    memset((BYTE*)&APDUBufFlag.Request.Ch,0x00, sizeof(APDUBufFlag));
    APDUBufFlag.Request.Flag = FALSE;
}

//--------------------------------------------------
//功能描述: APDUBufFlag初始化
//         
//参数  :    无
//
//返回值:     无   
//         
//备注内容:    无
//--------------------------------------------------
void InitAPDUBufFlag( BYTE Ch )
{
    if( APDUBufFlag.Request.Ch == Ch )
    {
        APDUBufFlag.Request.Flag = FALSE;
    }
}

//--------------------------------------------------
//功能描述: 获取各通道默认最大帧buf长度
//         
//参数  :    BYTE Ch[in] 通道
//
//返回值:     默认长度   
//         
//备注内容:    无
//--------------------------------------------------
DWORD GetDefaultConsultBufLen( BYTE Ch )
{
    if( Ch == eCR )//当前通道为载波通道
    {
       return DEFAULT_MAX_PRO_BUF_CR;
    }
    else if( Ch == eIR )//当前通道为红外通道
    {
      return DEFAULT_MAX_PRO_BUF_IR;
    }
    else//当前通道为485通道
    {
       return DEFAULT_MAX_PRO_BUF_485;
    }
}

//--------------------------------------------------
//功能描述: 获取各通道默认最大帧buf长度
//         
//参数  :    BYTE Ch[in] 通道
//
//返回值:     协商长度   
//         
//备注内容:    无
//--------------------------------------------------
DWORD GetConsultBufLen(   BYTE Ch )
{
   DWORD ConsultLen;
   
   ConsultLen = GetDefaultConsultBufLen(Ch);
   
    if( (api_GetSysStatus( eSYS_STATUS_ID_698MASTER_AUTH ) == TRUE)
	||(api_GetSysStatus( eSYS_STATUS_ID_698TERMINAL_AUTH ) == TRUE) 
	||(api_GetSysStatus( eSYS_STATUS_ID_698GENERAL_AUTH ) == TRUE))//通道相符且建立应用连接成功
    {
        if( ConsultData.channel == Ch )//正常情况下、通道肯定相等
        {
            if( ConsultData.Len >= MAX_PRO_BUF )//超过物理空间长度使用最大长度
            {
                return MAX_PRO_BUF;
            }
            else
            {
                if( ConsultData.Len < MIN_CONSULT_BUF_LEN )//认为通道协商长度不能低于100，低于100使用默认长度
                {
                    ConsultData.Len = ConsultLen;
                }
                
                return ConsultData.Len;
            }
        }
        else//异常情况
        {
           return ConsultLen;
        }
    }
    else
    {
       return ConsultLen;
    }
}

//--------------------------------------------------
//功能描述:  主循环清零函数
//         
//参数  :   无
//
//返回值:    无   
//         
//备注内容:  每500毫秒调用一次
//--------------------------------------------------
void ProcClrTask(void)
{
	WORD wTmpClearMeterType;

	//wClearMeterType //0x1111--电表清零(数据初始化) 0x2222--恢复出厂参数 0x4444--事件清零 0x8888--需量清零
	if( FPara1->wClearMeterType == 0 )
	{
		return;
	}
	
	if( g_byClrTimer_500ms != 0 )//清零500毫秒定时器，上电置0
	{
		g_byClrTimer_500ms--;
		return;
	}
	
	//如果参数区1校验错误，则退出，不执行清零，防止新EEPROM上电总在清零
	if( lib_CheckSafeMemVerify( (BYTE *)(FPara1), sizeof(TFPara1), UN_REPAIR_CRC ) == FALSE )
	{	
		return;
	}
	
	wTmpClearMeterType = FPara1->wClearMeterType;

	// 电表清零时，清需顺序：清电能--清需量--清冻结数据--清负荷曲线--清事件
	// 清事件放在最后，这样做是为了兼容事件清零命令

	//用预置卡清零，先清零，这样返写卡时电能数据都是0，这样卡表此处不会再清电能
	//远程表没有卡表返写功能，先应答，后清零，本地费控表用钱包初始化命令时没有返写，也先应答，后清零
	
	if( (wTmpClearMeterType == eCLEAR_METER) || (wTmpClearMeterType == eCLEAR_MONEY) )//方法3. 数据初始化 按以前 电表清零 做
	{
		ClearMeter( );	
		CLEAR_WATCH_DOG;
		#if( PREPAY_MODE == PREPAY_LOCAL )
		if(wTmpClearMeterType == eCLEAR_MONEY)//eCLEAR_MONEY   = 0x6666,		//钱包初始化或参数预置卡
		{
			api_SavePrgOperationRecord( ePRG_BUY_MONEY_NO ); //购电记录有购电前数据、购电后数据，能不能参考逆相序做，但购电前后数据排名比较乱
		}		
		#endif
	}
	else if( wTmpClearMeterType == eCLEAR_FACTORY )//初始化参数
	{
		ClearMeter( );		
		InitPara( 1 );
		api_FactoryInitTask( );
	}
	else if( wTmpClearMeterType == eCLEAR_EVENT )//方法5. 事件初始化 由于事件清零记录是特殊记录 含有事件清零列表 暂将事件清零分配到具体的清零中去
	{
		api_ClrAllEvent(eEXCEPT_CLEAR_METER_CLEAR_EVENT);
	}	
	else if( wTmpClearMeterType == eCLEAR_DEMAND )//方法6. 需量初始化 按以前 需量清零 做
	{
	
		#if( MAX_PHASE == 3 )
		#if( SEL_DEMAND_2022 == NO )
		api_ClrDemand(eClearAllDemand);
		#else
		api_ClrDemand(eClearAllDemand,eDemandPause);
		#endif
		#endif
	}
	
	wTmpClearMeterType = 0;
		
	api_WritePara(0, GET_STRUCT_ADDR(TFPara1, wClearMeterType), sizeof(WORD), (BYTE *)&wTmpClearMeterType );
}

//--------------------------------------------------
//功能描述:    根据时间间隔转化分钟数
//         
//参数:      DWORD AbsMin[in]	            //初始分钟数
//         
//         BYTE TIIndex[in]			    //TI的间隔
//         
//         WORD TI[in]				    //间隔值
//         
//返回值:     DWORD 绝对时间分钟数
//         
//备注内容:    此处TIIndex未判断必须小于6 使用时务必保证小于6，避免错误
//--------------------------------------------------
static DWORD TimeIntervalToAbsMin( DWORD AbsMin, BYTE TIIndex, WORD TI )
{
	TRealTimer  TmpRealTimer ;
	WORD Result;
	DWORD AbsTime698;
	
	AbsTime698 = 0;
	
	if( TIIndex == 0 )	//时间间隔是秒，则直接返回分钟数，冻结不存在秒冻结
	{
		return AbsMin+(TI/60);
	}
	else if( TIIndex < 4)	//时间间隔是 分 时 日 时 根据数据返回时间数
	{
		return (AbsMin+TIMinutes[TIIndex]*TI);
	}
	else if( TIIndex  == 4) //时间间隔为月
	{
		Result = api_ToAbsTime( AbsMin, &TmpRealTimer);//将绝对时间转化为相对时间
		if( Result == TRUE )
		{
			if( (TmpRealTimer.Mon + TI) > 12 )//月需要进位
			{
				TmpRealTimer.wYear += (TmpRealTimer.Mon + TI)/12;//先算年后算月，顺序不能更改
				TmpRealTimer.Mon = (TmpRealTimer.Mon + TI)%12;
			
			}
			else
			{
				TmpRealTimer.Mon = TmpRealTimer.Mon + TI;
			}
			
			AbsTime698 = api_CalcInTimeRelativeMin( &TmpRealTimer );//转换为绝对时间
		}
	}
	else if( TIIndex == 5 ) //时间间隔为年
	{
		Result = api_ToAbsTime( AbsMin, &TmpRealTimer ); //将绝对时间转化为相对时间
		if( Result == TRUE )
		{
			TmpRealTimer.wYear = TmpRealTimer.wYear + TI;
			AbsTime698 = api_CalcInTimeRelativeMin( &TmpRealTimer );//转换为绝对时间
		}
	}
	else
	{
        AbsTime698 = 0;
	}

	return AbsTime698;
}

//--------------------------------------------------
//功能描述:    根据时间间隔转化分钟数
//         
//参数:      DWORD AbsMin[in]	            //初始分钟数
//         
//         BYTE TIIndex[in]			    //TI的间隔
//         
//         WORD TI[in]				    //间隔值
//         
//返回值:     DWORD 绝对时间分钟数
//         
//备注内容:    此处TIIndex未判断必须小于6 使用时务必保证小于6，避免错误
//--------------------------------------------------
static DWORD TimeIntervalToAbsSec( DWORD AbsSec, BYTE TIIndex, WORD TI )
{
	TRealTimer  TmpRealTimer ;
	WORD Result;
	DWORD AbsTime698,AbsMin,Sec;
	
	AbsTime698 = 0;
	AbsMin = (AbsSec/60);	//获取相对分钟数
	Sec = (AbsSec%60);		//获取秒数
	
	if( TIIndex == 0 )	//时间间隔是秒，则直接返回分钟数，冻结不存在秒冻结
	{
		return AbsSec+TI;
	}
	else if( TIIndex < 4)	//时间间隔是 分 时 日 时 根据数据返回时间数
	{
		return (AbsSec+(TIMinutes[TIIndex]*TI)*60);
	}
	else if( TIIndex == 4) //时间间隔为月
	{
		Result = api_ToAbsTime( AbsMin, &TmpRealTimer);//将绝对时间转化为相对时间
		if( Result == TRUE )
		{
			if( (TmpRealTimer.Mon + TI) > 12 )//月需要进位
			{
				TmpRealTimer.wYear += (TmpRealTimer.Mon + TI - 1) / 12;//先算年后算月，顺序不能更改
				TmpRealTimer.Mon = (TmpRealTimer.Mon + TI - 1) % 12 + 1;
				if( TmpRealTimer.wYear > 2099 )//极限值判断，不允许超过99年
				{
					TmpRealTimer.wYear = 2099;
				}
			
			}
			else
			{
				TmpRealTimer.Mon = TmpRealTimer.Mon + TI;
			}
			TmpRealTimer.Sec =Sec;
			AbsTime698 = api_CalcInTimeRelativeSec( &TmpRealTimer );//转换为绝对时间
		}
	}
	else if( TIIndex == 5 ) //时间间隔为年
	{
		Result = api_ToAbsTime( AbsMin, &TmpRealTimer ); //将绝对时间转化为相对时间
		if( Result == TRUE )
		{
			TmpRealTimer.wYear = TmpRealTimer.wYear + TI;
			if( TmpRealTimer.wYear > 2099 )//极限值判断，不允许超过99年
			{
				TmpRealTimer.wYear = 2099;
			}
			
			TmpRealTimer.Sec =Sec;
			AbsTime698 = api_CalcInTimeRelativeSec( &TmpRealTimer );//转换为绝对时间
		}
	}
	else
	{
        AbsTime698 = 0;
	}

	return AbsTime698;
}

//--------------------------------------------------
//功能描述:       判断广播校时是否过结算日
//         
//参数  :       AbsSetSecTime:广播校时后绝对时间
//
//返回值:        TRUE
//			  FALSE
//备注内容: 
//--------------------------------------------------
BOOL JudgeBroadcastTime( DWORD AbsSetSecTime)
{
	BYTE i;
	DWORD RealSecTime,CheckSecTime;
	WORD Result;
	TBillingPara BillingPara;
	TRealTimer CheckTimer;
	#if( PREPAY_MODE == PREPAY_LOCAL )
	TLadderBillingPara LadderBillingPara;//当前阶梯电价表

	api_GetCurLadderYearBillInfo( &LadderBillingPara );
	#endif
	
	Result = ReadSettlementDate((BYTE *)&BillingPara);	//结算日
	if( Result == FALSE )
	{
		return TRUE;
	}
	
	for(i = 0;i < MAX_MON_CLOSING_NUM;i++)
	{
		if( 	( BillingPara.MonSavePara[i].Day <= 28)
			&&	( BillingPara.MonSavePara[i].Hour <= 23 ) )
		{
			CheckTimer.Sec = 0;
			CheckTimer.Min = 0;
			CheckTimer.Day = BillingPara.MonSavePara[i].Day;
			CheckTimer.Hour = BillingPara.MonSavePara[i].Hour;
			CheckTimer.Mon = RealTimer.Mon;
			CheckTimer.wYear = RealTimer.wYear;
			RealSecTime = api_CalcInTimeRelativeSec( &RealTimer );
			CheckSecTime = api_CalcInTimeRelativeSec(&CheckTimer);
			//采用秒判断方式，不能采用分钟,颗粒度太低
			if((AbsSetSecTime>RealSecTime))
			{
				if (  (CheckSecTime>RealSecTime)
					&&(CheckSecTime<=AbsSetSecTime))
				{
					return FALSE;
				}
			}
			else
			{
				if (  (CheckSecTime>AbsSetSecTime)
					&&(CheckSecTime<=RealSecTime))
				{
					return FALSE;
				}
			}
		}
	}


	#if( PREPAY_MODE == PREPAY_LOCAL )
	for(i=0;i<MAX_YEAR_BILL;i++)
	{
		if(  ( LadderBillingPara.LadderSavePara[i].Day <= 28 )
		&&	( LadderBillingPara.LadderSavePara[i].Hour <= 23 ) )
		{
			CheckTimer.Sec = 0;
			CheckTimer.Min = 0;
			CheckTimer.Day = LadderBillingPara.LadderSavePara[i].Day;
			CheckTimer.Hour = LadderBillingPara.LadderSavePara[i].Hour;
			
			if( LadderBillingPara.LadderSavePara[i].Mon > 12 )//非法值---月阶梯
			{
				CheckTimer.Mon = RealTimer.Mon;
			}
			else
			{
				CheckTimer.Mon = LadderBillingPara.LadderSavePara[i].Mon;
			}

			CheckTimer.wYear = RealTimer.wYear;
			RealSecTime = api_CalcInTimeRelativeSec( &RealTimer );
			CheckSecTime = api_CalcInTimeRelativeSec(&CheckTimer);
			//采用秒判断方式，不能采用分钟,颗粒度太低
			if((AbsSetSecTime>RealSecTime))
			{
				if (  (CheckSecTime>RealSecTime)
					&&(CheckSecTime<=AbsSetSecTime))
				{
					return FALSE;
				}
			}
			else
			{
				if (  (CheckSecTime>AbsSetSecTime)
					&&(CheckSecTime<=RealSecTime))
				{
					return FALSE;
				}
			}
			
			if( (i == 0) && (LadderBillingPara.LadderSavePara[i].Mon > 12) )
			{
				break;
			}
			
		}
	}
	#endif

	return TRUE;
}


//--------------------------------------------------
//功能描述:       判断广播校时是否发生在结算时刻前后5分钟
//         
//参数  :       无
//
//返回值:        TRUE--超过5min    
//			  FALSE--5min以内
//备注内容: 
//--------------------------------------------------
BOOL JudgeBroadcastMeterTime( WORD Sec )
{
	BYTE i;
	DWORD TempSecCheck;
	WORD Result;
	TBillingPara BillingPara;
	TRealTimer CheckTimer;
	#if( PREPAY_MODE == PREPAY_LOCAL )
	TLadderBillingPara LadderBillingPara;//当前阶梯电价表

	api_GetCurLadderYearBillInfo( &LadderBillingPara );
	#endif
	
	Result = ReadSettlementDate((BYTE *)&BillingPara);	//结算日
	if( Result == FALSE )
	{
		return TRUE;
	}
	
	for(i = 0;i < MAX_MON_CLOSING_NUM;i++)
	{
		if( 	( BillingPara.MonSavePara[i].Day <= 28)
			&&	( BillingPara.MonSavePara[i].Hour <= 23 ) )
		{
			CheckTimer.Sec = 0;
			CheckTimer.Min = 0;
			CheckTimer.Day = BillingPara.MonSavePara[i].Day;
			CheckTimer.Hour = BillingPara.MonSavePara[i].Hour;
			CheckTimer.Mon = RealTimer.Mon;
			CheckTimer.wYear = RealTimer.wYear;
			TempSecCheck = api_CalcInTimeRelativeSec( &RealTimer );
			//采用秒判断方式，不能采用分钟,颗粒度太低
			if( labs(TempSecCheck - api_CalcInTimeRelativeSec(&CheckTimer)) < Sec)
			{
				return FALSE;
			}
		}
	}


	#if( PREPAY_MODE == PREPAY_LOCAL )
	for(i=0;i<MAX_YEAR_BILL;i++)
	{
		if(  ( LadderBillingPara.LadderSavePara[i].Day <= 28 )
		&&	( LadderBillingPara.LadderSavePara[i].Hour <= 23 ) )
		{
			CheckTimer.Sec = 0;
			CheckTimer.Min = 0;
			CheckTimer.Day = LadderBillingPara.LadderSavePara[i].Day;
			CheckTimer.Hour = LadderBillingPara.LadderSavePara[i].Hour;
			
			if( LadderBillingPara.LadderSavePara[i].Mon > 12 )//非法值---月阶梯
			{
				CheckTimer.Mon = RealTimer.Mon;
			}
			else
			{
				CheckTimer.Mon = LadderBillingPara.LadderSavePara[i].Mon;
			}

			CheckTimer.wYear = RealTimer.wYear;
			TempSecCheck = api_CalcInTimeRelativeSec( &RealTimer );
			//采用秒判断方式，不能采用分钟,颗粒度太低
			if( labs(TempSecCheck - api_CalcInTimeRelativeSec(&CheckTimer)) < Sec )
			{
				return FALSE;
			}
			
			if( (i == 0) && (LadderBillingPara.LadderSavePara[i].Mon > 12) )
			{
				break;
			}
			
		}
	}
	#endif

	return TRUE;
}

//--------------------------------------------------
//功能描述:  判断以698为核心架构软件的时段表间隔是否大于15分钟
//         
//参数  :   BYTE *pBuf[in] 时段表BUF指针
//			pBuf[0]--当前时段的时 pBuf[1]--当前时段的分 pBuf[2]--当前时段的费率号
//			pBuf[3]--下一时段的时 pBuf[4]--下一时段的分 pBuf[5]--下一时段的费率号
//
//
//返回值:    WORD TRUE 有效  FALSE 无效 
//
//备注内容:  以698为核心架构软件的时段传输顺序:pBuf[0]--时段时 pBuf[1]--时段分 pBuf[2]--时段费率号，双协议表中645调用本函数前也必须按此顺序，
//			 注意以前以645为核心架构的软件时段传输顺序正好和698顺序相反，此函数和以前以645为核心架构的软件中顺序不同
//			 另外698为核心架构软件的时区传输顺序:pBuf[0]--时区月 pBuf[1]--时区日 pBuf[2]--时段表号，也和以前以645为核心架构的软件中顺序不同
//--------------------------------------------------
WORD api_JudgeTimeSegInterval(BYTE *pBuf)
{
	WORD wIntervalRightValue;
	int iInterval;
	
	wIntervalRightValue = 15;//时段间隔不能小于15分钟

	iInterval = (pBuf[3]-pBuf[0]);//下一时段和当前这个时段差几个小时

	iInterval *= 60;//下一时段和当前这个时段间隔转化为以分钟为单位

	iInterval = (iInterval + pBuf[4]-pBuf[1]) ;//时段间隔加上下一时段和当前这个时段的分钟差

	#if( MAX_PHASE != 1)
	//新国网要求最小时段不能小于需量周期
	wIntervalRightValue = (FPara2->EnereyDemandMode.DemandPeriod);
	if( wIntervalRightValue < 15 )
	{
		wIntervalRightValue = 15;
	}
	#endif

	//如果时段间隔为0，则认为是补足的，要求下一时段和当前这个时段的费率也一样
	if( ( iInterval == 0 )&&(pBuf[2] == pBuf[5] ) )
	{
		return TRUE;
	}
	else if( iInterval >= wIntervalRightValue )//此处int和WORD比较，先将WORD转成int再比较。这个地方比较方法不推荐
	{
		return TRUE;
	}
	else//包括间隔 < 15，间隔 iInterval < 0 （即可能第一时段10点，第二个时段9点）
	{
		return FALSE;
	}
}

//--------------------------------------------------
//功能描述:    建立应用连接时效判断
//         
//参数:      BYTE Ch[in]		        //通道选择
//         eRequestMode eType[in]	//0x00: 代表SET  0x55: 代表ACTION
//
//返回值:     BOOL  TRUE 有效  FALSE 无效
//         
//备注内容:  无
//--------------------------------------------------
static BOOL JudgeConnectValid( BYTE Ch,eRequestMode eType )
{
    //if( Ch == eIR )
    //{
    //    if( api_GetSysStatus( eSYS_IR_ALLOW_PRG ) != TRUE )//红外认证不通过不可进行后续操作
    //    {
    //        return FALSE;
    //    }
    //}
		
	return TRUE;
}
//--------------------------------------------------
//功能描述:红外端口权限判断
//         
//参数:      BYTE Ch[in]		        //通道选择
//         eRequestMode eType[in]	//请求模式
//         WORD OI                  //需要判断的OI
//返回值:     BOOL  TRUE 有效  FALSE 无效 //FALSE不是失败，代表需要继续进行后续的判断
//         
//备注内容:  无
//--------------------------------------------------
static BOOL JudgeIRAuthority( BYTE Ch,eRequestMode eType,BYTE* pOMD )
{
    TFourByteUnion OMD;

    lib_ExchangeData( OMD.b, pOMD, 4 );//获取OMD 
    if( eType == eACTION_MODE )//支持其他通道 红外请求与认证允许不建立应用连接操作
    {
        //红外请求与认证允许不建立应用连接操作
        //左移8位 不判断OMD的操作模式
        if( ((OMD.d >>8) == 0X00F1000B) ||((OMD.d >>8) == 0X00F1000C) )
        {
            return TRUE;
        }
    }
    /*if( Ch == eIR )
    {
        if( eType == eREAD_MODE )
        {
            if( api_GetSysStatus( eSYS_IR_ALLOW_PRG ) == FALSE )//不建立应用连接支持读取有限数据
            {
                if(    (OMD.w[1] == 0x4002) //表号
                    || (OMD.w[1] == 0x4001) //通信地址
                    || (OMD.w[1] == 0x4111) //备案号
                    || (OMD.w[1] == 0x4000) //当前日期时间
                    || (OMD.w[1] == 0x202C))//当前钱包文件
                 {
                    return TRUE;
                 }

                if( (OMD.b[3] >= 0x00) && (OMD.b[3] <= 0x05) ) //支持读取所有电能
                {
                    return TRUE;
                }
            }
            
        }
    }*/
   
    return FALSE;
}

//--------------------------------------------------
//功能描述:    安全模式参数判断
//         
//参数:      BYTE Ch[in]			//通道选择
//         eRequestMode eType	//请求方式
//         BYTE *pOI[in] 		//OI指针
//         
//返回值:     WORD  TRUE 有效   FALSE 无效
//         
//备注内容:    无
//--------------------------------------------------
static WORD JudgeSafeModePara(BYTE Ch,eRequestMode eType , BYTE *pOI)
{
	BYTE i, OIA;
	BYTE Cmd, SecurityMode;
	WORD wSafeMode;
	TTwoByteUnion OI;

	if( eType == eREAD_MODE )//根据输入的操作类型进行判断cmd，解决actionthenget权限判断不对的问题2
	{
		Cmd = 0x05;
	}
	else if( eType == eSET_MODE )
	{
		Cmd = 0x06;
	}
	else if( eType == eACTION_MODE )
	{
		Cmd = 0x07;
	}
	else
	{
		Cmd = LinkData[Ch].pAPP[0];
	}
	
	SecurityMode = APPData[Ch].eSafeMode;
	lib_ExchangeData( OI.b, pOI, 2);
	
	//判断显示安全模式参数	
	for( i=0; i<SafeModePara.Num; i++ )
	{
		if( OI.w == SafeModePara.Sub[i].OI )
		{
			//这个OI在显示安全模式参数中，则判断权限是否一致
			switch(Cmd)
			{
			case 0x05://DealGet_Request
			case 0x06://DealSet_Request
			case 0x07://DealAction_Request
				wSafeMode = SafeModePara.Sub[i].wSafeMode;
				wSafeMode = ((wSafeMode>>((8-Cmd)*4))&0x0f);
				//至此safemode中bit3~bit0:明文方式操作	明文+数据验证码操作	密文方式操作	密文+数据验证码操作
				if( (wSafeMode==0) || (wSafeMode==0xf) )//安全模式同时包含明文、明文+MAC、密文、密文+MAC四种，则认为安全模式乱，不判断
				{
					return TRUE;
				}
				
				if( SecurityMode == eNO_SECURITY )
				{
					if( wSafeMode & 0x8 )
					{
						return TRUE;
					}	
				}		
				else if( (SecurityMode==eEXPRESS_MAC) || (SecurityMode==eSECURITY_RN) )
				{
					if( Ch != eIR )//红外通道不进行身份认证启用标志的判断
					{
						if( (api_GetRunHardFlag(eRUN_HARD_ESAM_ERR) == FALSE)&&(Cmd == 0x05)&&(wSafeMode&0x04)&&(api_GetRunHardFlag( eRUN_HARD_EN_IDENTAUTH ) == TRUE) )
						{
							//ESAM正常状态下，GET的安全模式为明文+数据验证码，并且身份认证启用，需要建立应用连接或者终端身份认证抄读数据
							if( CURR_COMM_TYPE(Ch) == COMM_TYPE_TERMINAL )
							{
								if( (api_GetSysStatus( eSYS_STATUS_ID_698TERMINAL_AUTH ) == FALSE)&&(api_GetSysStatus( eSYS_TERMINAL_ALLOW_PRG ) == FALSE) )
								{
									return FALSE;
								}
							}
							else if( api_GetSysStatus( eSYS_STATUS_ID_698MASTER_AUTH ) == FALSE )
							{
								return FALSE;
							}
							
							return TRUE;
						}
					}

					if( wSafeMode&0xc ) //明文、明文+MAC
					{
						return TRUE;
					}	
				}
				else if( SecurityMode == eSECRET_TEXT )
				{
					if( wSafeMode & 0xe )//明文、明文+MAC、密文
					{
						return TRUE;
					}
				}
				else if(  SecurityMode == eSECRET_MAC )
				{
					//if( wSafeMode & 0xf )
					{
						return TRUE;
					}	
				}	
				break;	
			case 0x10://DealSecurity_Request
				return TRUE;
				break;		
			default://其它命令
				return TRUE;
				break;
			}
			return FALSE;
		}	
	}
	
	OIA = (OI.w/0x100);
	
	//判断默认安全模式参数,
	switch(Cmd)
	{
	case 0x05://DealGet_Request
		//if( wResult = );
		if( (OI.w/0x1000) == 0 )//电能默认明文可读
		{
			return TRUE;
		}
		if( OI.w == 0x202c )//当前钱包文件
		{
			return TRUE;
		}
	
		//4000	日期时间	√ 4001	通信地址	√ 4002	表号	√
		if( (OI.w >= 0x4000)&&(OI.w <= 0x4002) )
		{
			return TRUE;
		}
		if (OI.w == 0xF221)
		{
			return TRUE;
		}
		if( (OI.w >= 0x6000)&&(OI.w <= 0x60FF) )
		{
			return TRUE;
		}
		/*
		F000	分帧传输	√
		F001	分块传输	√
		F002	扩展传输	√
		F100	ESAM	√
		F101	安全模式参数	√
		F2ZZ	输入输出接口设备	√
		FFZZ	自定义	√
		*/
		if( OI.w == 0x4111 )//备案号
		{
			return TRUE;
		}

		if( (OIA == 0xf0)||(OIA == 0xf1)||(OIA == 0xf2)||(OIA == 0xff) )
		{
			return TRUE;
		}
		//除以上明文可读外，其它数据都是权限大于等于明文+MAC可读，
		//读取权限由低到高：明文 明文+MAC 密文 密文+MAC	，但设置时数据设置方式可能是指定的，例如表号只能用明文+MAC设置，用密文+MAC方式设置表号ESAM可能不支持设置到ESAM中去
		if( Ch != eIR )//红外通道不进行身份认证启用标志的判断
		{
			if( (api_GetRunHardFlag(eRUN_HARD_ESAM_ERR) == FALSE)&&(SecurityMode == eSECURITY_RN )&&(api_GetRunHardFlag( eRUN_HARD_EN_IDENTAUTH ) == TRUE))
			{
				if( CURR_COMM_TYPE(Ch) == COMM_TYPE_TERMINAL)
				{
					if( (api_GetSysStatus( eSYS_STATUS_ID_698TERMINAL_AUTH ) == FALSE)&&(api_GetSysStatus( eSYS_TERMINAL_ALLOW_PRG ) == FALSE) )
					{
						return FALSE;
					}
				}
				else if( api_GetSysStatus( eSYS_STATUS_ID_698MASTER_AUTH ) == FALSE )
				{
					return FALSE;
				}
			}
		}

		if( SecurityMode >= eSECURITY_RN )
		{
			return TRUE;
		}
		break;	
	case 0x06://DealSet_Request
		//4002	表号, 4003 客户编号, F100 ESAM 默认设置方式：明文+MAC 
		if( (OI.w == 0x4002)||(OI.w == 0x4003)||(OI.w == 0xf100) )
		{
			if( SecurityMode >= eEXPRESS_MAC )
			{
				return TRUE;
			}
		}
		else if( (OIA == 0xf2)||(OIA == 0xff) )//F2ZZ	输入输出接口设备	√ 	FFZZ	自定义
		{
			return TRUE;
		}
		else if( (OI.w >= 0x6000)&&(OI.w <= 0x60FF) )
		{
			return TRUE;
		}
		//其它数据都是权限等于密文+MAC可设置	
		if( SecurityMode == eSECRET_MAC )
		{
			return TRUE;
		}		
		break;	
	case 0x07://DealAction_Request
		if( (OI.w == 0x4000) || (OI.w == 0x2015) )//4000 日期时间 2015 跟随上报状态字
		{
			return TRUE;
		}
		else if( OI.w == 0x5000 )//瞬时冻结，明文+MAC
		{
			if( SecurityMode >= eEXPRESS_MAC )
			{
				return TRUE;
			}
		}
        // else if( OI.w == 0xf001 )//F001	文件分块传输
		// {
		// 	return TRUE;
		// }
		else if( OI.w == 0xf002 )//F002	扩展传输
		{
			return TRUE;
		}
		else if( (OIA >= 0x20) && (OIA <= 0x2f) )//2ZZZ	变量借口类
		{
		    if( SecurityMode >= eEXPRESS_MAC )
			{
				return TRUE;
			}
		}
		else if( (OIA == 0xf2)||(OIA == 0xff) )//F2ZZ	输入输出接口设备	√ 	FFZZ	自定义
		{
			return TRUE;
		}
		else if( (OI.w == 0xf001)||(OI.w == 0xf101) )//操作方式：密文 的有 F001	分块传输 F101	安全模式参数 
		{
			if( SecurityMode >= eSECRET_TEXT )
			{
				return TRUE;
			}
		}
		else if( (OI.w >= 0x6000)&&(OI.w <= 0x60FF) )
		{
			return TRUE;
		}
		else if(OI.w ==0xfefe )
		{
			return TRUE;
		}
		//如果权限等于密文+MAC都可操作	
        if( SecurityMode == eSECRET_MAC )
        {
          return TRUE;
        }
		break;	
	//case 0x10://DealSecurity_Request
	//	break;		
	default:
		break;
	}
	
	return FALSE;
}

//--------------------------------------------------
//功能描述:  建立应用连接权限判断
//         
//参数:     BYTE Ch[in]		        //通道选择
//        eRequestMode eType[in]	//0x00: 代表SET  0x55: 代表ACTION
//        BYTE* pOI                 //OI指针
//
//返回值:    WORD  TRUE 有效  FALSE 无效
//         
//备注内容:  无
//--------------------------------------------------
WORD JudgeTaskAuthority( BYTE Ch, eRequestMode eType, BYTE *pOI )
{
	TTwoByteUnion OI;
	WORD Result;
	BYTE ClassAttribute;
	
	if( api_GetSysStatus( eSYS_STATUS_INSIDE_FACTORY ) == TRUE ) 
	{
		return TRUE;
	}

	//if( (CURR_COMM_TYPE( Ch ) == COMM_TYPE_TERMINAL)&&(Ch == eIR) )
	//{
	//	//eIR的客户机地址只能是主站地址
	//	return FALSE;
	//}
	
	lib_ExchangeData( OI.b, pOI, 2 ); //获取OI
	ClassAttribute = pOI[2];	  //获取属性
		
	//操作时间(广播校时)、操作多功能端子、操作随时冻结可以不进行应用连接直接操作
	//红外认证后支持
	if( (eType==eACTION_MODE) && (LinkData[Ch].AddressType != eWILDCARD_ADDRESS_TYPE) )
	{
		if(		(OI.w == 0X4000)//日期时间 
			|| 	(( OI.w & 0xFF00) == 0xF200)//多功能端子 
			//|| 	(( OI.w & 0xFF00) == 0xFE00) 
			|| 	(( OI.w & 0xFF00) == 0x6000)//操作档案
			|| 	( (OI.w==0x5000) && (ClassAttribute==3) )//广播冻结
			|| 	( (OI.w==0xF300) && ((ClassAttribute==5)||(ClassAttribute==6)) ) //显示查看与全显命令
			|| 	( (OI.w==0xF301) && ((ClassAttribute==5)||(ClassAttribute==6)) )//显示查看与全显命令
            || 	( (OI.w==0xF001) && (ClassAttribute==8))) 
		{
		    //if( (Ch == eIR) && (api_GetSysStatus( eSYS_IR_ALLOW_PRG ) != TRUE) )//红外不进行红外认证不支持操作
    		//{
            //    return FALSE;
    		//}
            if( (OI.w == 0X4000) 
              && (APPData[Ch].eSafeMode != eNO_SECURITY) 
              && (api_GetRunHardFlag(eRUN_HARD_MAC_ERR_FLAG) == TRUE) 
            )
            {
                //电能表处于挂起时，不允许使用安全模式广播校时
                return FALSE;
            }
    		
			return TRUE;
		}
	}

    if( (eType != eREAD_MODE) && (LinkData[Ch].AddressType == eSINGLE_ADDRESS_TYPE) )//明文合闸只支持单地址
    {
        if( eType == eACTION_MODE ) 
        {
            if( (OI.w==0x8000) && (ClassAttribute==131) ) //明文合闸 支持明文
            {
                return TRUE;
            }
            else if( (OI.w==0x2015) && (ClassAttribute==127) )//确认跟随上报状态字
            {
                //if( (Ch == eIR) && (api_GetSysStatus( eSYS_IR_ALLOW_PRG ) != TRUE) )//红外不进行红外认证不支持操作
                //{
                //    return FALSE;
               // }
                
                return TRUE;
            }
            else{} 
        }
        else
        {
            if( (OI.w==0x4500) || (OI.w==0x4501) )//设置公网通信模块1、2
            {
                if( Ch == eCR )
                {
                    return TRUE;
                }
            }
        }

    }
    
    if( (api_GetRunHardFlag(eRUN_HARD_MAC_ERR_FLAG) == TRUE)//挂起不允许操作与设置
    && (eType!=eREAD_MODE) )
    {
		return FALSE;
    }
    
	//除单地址外只允许读数据 与上面代码的顺序不能颠倒!!!!!!
	if( (LinkData[Ch].AddressType!=eSINGLE_ADDRESS_TYPE) && (eType!=eREAD_MODE) )
	{
		return FALSE;
	}
	
	Result = JudgeIRAuthority( Ch, eType, pOI );
	if( Result == TRUE )//失败代表正常操作 需要继续进行后续判断
	{
        return TRUE;
	}
	
	Result = JudgeConnectValid( Ch, eType );
	if( Result  == FALSE )
	{
		return FALSE;
	}
#if((SEL_JUDGE_SAFE_MODE == YES) &&(SEL_ESAM_TYPE != 0))
	Result = JudgeSafeModePara( Ch, eType ,pOI);

	return Result;
#else
	return TRUE;
#endif
}

//--------------------------------------------------
//功能描述:  判断时间标签函数
//         
//参数:     BYTE Ch[in]
//         
//返回值:   TRUE 有效  FALSE 无效  
//         
//备注内容:  无
//--------------------------------------------------
BOOL JudgeTimeTagValid(BYTE Ch)
{
	DWORD dwTime,dwRealTime;//unsigned long int
	BYTE TIIndex;
	WORD wTI,Len;
	BYTE *pBuf;
	TRealTimer TimeTag,tmpRealTimer;
	
	if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return FALSE;
    }

	Len = AnalyseDlt698ApduLen( Ch );
	if( Len == 0x8000 )
	{
       return FALSE;
	}
	
	pBuf = (BYTE *)&(LinkData[Ch].pAPP[Len]);
	LinkData[Ch].pAPPLen.w -= 1;//剔除OPTIONAL 1个字节

	if( Len > LinkData[Ch].pAPPLen.w )//防止字节数解析超限导致时间标签误判
	{
	    APPData[Ch].TimeTagFlag = eNO_TimeTag;//默认无时间标签
        return TRUE;
	}
	
	if( pBuf[0] != 0x01 )//OPTINAL 为0 代表无时间标签
	{
		APPData[Ch].TimeTagFlag = eNO_TimeTag;//默认无时间标签
	}
	else
	{
	    memcpy( g_TimeTagBuf, pBuf+1, 10 );//保存时间标签
	    
	    if( (LinkData[Ch].pAPPLen.w -Len) < 10 )//剩余字节不够时间标签长度
	    {
            APPData[Ch].TimeTagFlag = eTime_Invalid;//时间标签无效
            return TRUE;
	    }
	    
	    LinkData[Ch].pAPPLen.w -= (LinkData[Ch].pAPPLen.w -Len);//应用层长度 剔除时间标签
       
        APPData[Ch].TimeTagFlag = eTime_Invalid;//时间标签无效
        
        memcpy( (BYTE*)&TimeTag.wYear, (BYTE*)&pBuf[1],7);
        lib_InverseData((BYTE*)&TimeTag.wYear, 2);
        
        if( api_GetRunHardFlag( eRUN_HARD_CLOCK_BAT_LOW ) == TRUE )//电池电压低默认时间标签有效
        {
            APPData[Ch].TimeTagFlag = eTime_True;//时间标签有效
            return TRUE;
        }
        
        if( api_CheckClock( (TRealTimer*)&TimeTag ) == FALSE )//时间格式错误
        {
            APPData[Ch].TimeTagFlag = eTime_Invalid;//时间标签无效
            return TRUE;
        }
        
        TIIndex = pBuf[1+7];
        if( TIIndex > 5 )
        {
            APPData[Ch].TimeTagFlag = eTime_Invalid;//时间标签无效
            return TRUE;
        }
        
        lib_ExchangeData((BYTE*)&wTI, (BYTE*)&pBuf[1+7+1], 2);
        
        api_GetRtcDateTime( DATETIME_20YYMMDDhhmmss, (BYTE *)&tmpRealTimer );
		dwRealTime = api_CalcInTimeRelativeSec(&tmpRealTimer );//获取当前时间的相对秒数，提高精度
        if( dwRealTime == ILLEGAL_VALUE_8F )//当前时间秒数错误，默认时间有效
        {
            APPData[Ch].TimeTagFlag = eTime_True;//时间有效;
            return TRUE;
        } 
        
        dwTime = api_CalcInTimeRelativeSec(&TimeTag);//获取下发时间相对秒数，提高精度
        if( dwTime == ILLEGAL_VALUE_8F )//CalcTime 内调用 api_CheckClock 没通过
        {
            APPData[Ch].TimeTagFlag = eTime_Invalid;//时间无效;
            return TRUE;
        }   
        if( dwTime >= dwRealTime )
        {
            APPData[Ch].TimeTagFlag = eTime_True;//有时间标签;
            return TRUE;
        }
        
        dwTime = TimeIntervalToAbsSec( dwTime, TIIndex, wTI );
        
        if( dwTime >= dwRealTime )
        {
            APPData[Ch].TimeTagFlag = eTime_True;//有时间标签;
            return TRUE;
        }
	}
	
	return TRUE;
}

//--------------------------------------------------
//功能描述:  698一般密码判断
//         
//参数:     BYTE* pBuf[in]
//         
//返回值:    BOOL  TRUE 有效  FALSE 无效   
//         
//备注内容:  无
//--------------------------------------------------
static BOOL Judge698PassWord( BYTE *pBuf )
{
	TMuchPassword  pPwd;
	BYTE i;
	
	//非厂内模式下如果电表闭锁则返回失败	
	if( 	( api_GetRunHardFlag(eRUN_HARD_PASSWORD_ERR_STATUS) == TRUE ) 
		&& 	( api_GetSysStatus(eSYS_STATUS_INSIDE_FACTORY) == FALSE ) )
	{
        return FALSE;
	}
	
	//读出EEPROM中保存的密码 不管读出是否成功后面都写，这样只有一次写不成功
	if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR(ParaSafeRom.MuchPassword), sizeof(TMuchPassword),(BYTE *)pPwd.Password645 ) != TRUE )
	{
		return TRUE;
	}	

	for( i=0; i< MAX_698PASSWORD_LENGTH; i++ )
	{
		if( pBuf[i] != pPwd.Password698[i] )
		{
		    g_PassWordErrCounter ++;
		    if( g_PassWordErrCounter >= 3 )
		    {
                api_SetRunHardFlag(eRUN_HARD_PASSWORD_ERR_STATUS);
		    }
			return FALSE;
		}
	}
	
    g_PassWordErrCounter = 0;

	return TRUE;
}

//--------------------------------------------------
//功能描述:    判断记录OAD的合法性
//         
//参数:      WORD OI[in]
//         
//         BYTE ClassAttribute[in]
//         
//返回值:     BOOL  TRUE 有效  FALSE 无效   
//         
//备注内容:    无
//--------------------------------------------------
static BOOL JudgeRecordOAD( WORD OI,BYTE ClassAttribute )
{
	//辅助电源掉电事件在宏定义未打开时不能进行读取
	if( (SelSecPowerConst==NO) && (OI==0x300e) )
	{
		return FALSE;
	}
	
	//接口类24
	if( ((OI>=0x3000)&&(OI<=0x3008)) || (OI==0x300b) || (OI==0x303B) ) 
	{
		if( 	( ClassAttribute != 6 ) 
			&& 	( ClassAttribute != 7 ) 
			&& 	( ClassAttribute != 8 ) 
			&& 	( ClassAttribute != 9 ) )
		{
			return FALSE;
		}
	}
	else if( ClassAttribute != 2 )
	{
		return FALSE;
	}

	return TRUE;
}

//--------------------------------------------------
//功能描述:    判断组帧长度 去除额外不能组帧的长度
//         
//参数:      BYTE Ch[in]
//         
//返回值:     BOOL  TRUE 有效  FALSE 无效   
//         
//备注内容:    无
//--------------------------------------------------
static void JudgeAppBufLen( BYTE Ch )
{
    if( APPData[Ch].APPCurrentBufLen.w > APPData[Ch].APPBufLen.w )//强制使数据长度相等，不影响后续数据处理
    {
        //ASSERT(FALSE, 0);
        APPData[Ch].APPCurrentBufLen.w = APPData[Ch].APPBufLen.w;
    }
    else
    {
	}
}

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
WORD AddProtocolAndFunctionConformance( BYTE Mode,BYTE *pBuf )
{
	BYTE Len;
	
	Len = 0;
	
    if( (Mode == 0) || (Mode == 1) )
    {
        memset( (void *)pBuf, 0x00, 8);
        //协议一致性(8字节)
        /*
        pBuf[0] = 0xf3;//(4)、(5)对应第1字节中bit3bit2清零
        pBuf[1] = 0xce;//(10)(11)(15)对应第2字节中bit5bit4bit0清零
        pBuf[2] = 0x2f;//(16)(17)(19)对应第3字节中bit7bit6bit4清零
        pBuf[3] = 0xe3;//(27)(28)(29)对应第4字节中bit4bit3bit2清零
        pBuf[4] = 0x80;
        */
        //技术要求中:期望的协议一致性块	FFFFFFFFC0000000H – 全部支持(bit0~bit33为1),同时要求:电表协议一致性块、功能一致性块，按默认参数应答，暂不要求协议、功能对应。
        pBuf[3] = 0x03;
        pBuf[4] = 0xff;
        pBuf[5] = 0xff;
        pBuf[6] = 0xff;
        pBuf[7] = 0xff;
      
        pBuf += 8;
        Len += 8;
    }
    
    if( (Mode == 0) || (Mode == 2) )
    {
        memset( (void *)pBuf, 0x00, 16);
        //功能一致性
        /*
        #if( MAX_PHASE == 1 )
        pBuf[0] = 0xc1;
        #else
        pBuf[0] = 0xed;//电能量计量(0) 双向有功计量(1) 无功电能计量(2) 有功需量(4) 无功需量(5) 复费率(7)
        #endif      
        
        #if( SEL_APPARENT_ENERGY == YES )
        pBuf[0] |= 0x12;//视在电能计量 视在需量
        #endif
        
        #if(SEL_RELAY_FUNC == YES)  
        pBuf[1] = 0x81;
        #if( PREPAY_MODE == PREPAY_LOCAL )
        pBuf[1] |= 0x40;
        #elif( PREPAY_MODE == PREPAY_REMOTE )
        pBuf[1] |= 0x20;
        #endif
        #else
        pBuf[1] = 0x01;
        #endif
        
        pBuf[2] = 0xf4; 
        
        */
        //技术要求:商定的功能一致性块	FFFEC400000000000000000000000000H – bit0-bit14，bit16，bit17，bit21
		pBuf[13] = 0x23;
		pBuf[14] = 0x7f;
		pBuf[15] = 0xff;
        Len += 16;
    }

    return Len;
}

//--------------------------------------------------
//功能描述:    时区时段表结构体数据处理
//         
//参数:      *pData[in] //pdata 为从array的个数处开始
//         
//返回值:     BOOL  TRUE 有效  FALSE 无效   
//         
//备注内容:    未进行极限值判断 务必保证传入数据正确 时+分+费率号
//--------------------------------------------------
static BOOL DealTimeZoneSegTableData( BYTE *pData )//时区时段表结构体数据处理
{
	BYTE i;
	BYTE *pSTData;//结构体数据指针
	
	for( i=0; i<pData[0]; i++ )
	{
		pSTData = &pData[1+8*i];

		if( pSTData[0] == Structure_698 )
		{
			if( pSTData[1] == 3)
			{
				if( pSTData[2] == Unsigned_698 )
				{
					ProtocolBuf[0+3*i] = pSTData[3];
					if( pSTData[4] == Unsigned_698 )
					{
						ProtocolBuf[1+3*i] = pSTData[5];
						if( pSTData[6] == Unsigned_698 )
						{
							ProtocolBuf[2+3*i] = pSTData[7];
						}
						else
						{
							break;
						}
					}
					else
					{
						break;
					}
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}

	if( i == pData[0] )
	{
		//后面补上第1时段，且小时加24，为判断最后一时段与第一时段间隔是否超过15min用，时区表也这么处理，后面不进行判断。
		ProtocolBuf[pData[0]*3]=(ProtocolBuf[0]+24);
		ProtocolBuf[pData[0]*3+1]=ProtocolBuf[1];	
		ProtocolBuf[pData[0]*3+2]=ProtocolBuf[2];
		return TRUE;
	}

	return FALSE;
}

//-----------------------------------------------
//函数功能:     698数据长度处理
//
//参数:		BYTE *buf[in]                   //数据BUF
//			BYTE len[in]                    //处理的数据长度
//			eDataLenTypeChoice eTypeChoice[in]： 
//              eUNION_TYPE          共用体模式          将Datalen（共用体数据）转换为符合协议长度的buf
//              ePROTOCOL_TYPE       协议模式           将符合协议长度的buf转换为Datalen（共用体数据）
//              eESAM_TYPE           ESAM模式         将符合esam长度的Datalen转换为符合协议长度的buf
//              e_UNION_OFFSET_TYPE  共用体偏移模式        返回根据Datalen长度转化为符合协议长度的buf的数据长度（buf不填数据）
//         
//返回值:	    BYTE 数据长度
//		   
//备注:       698数据长度处理，主要用来处理数据长度超过一个字节的情况 遇到优化问题,此函数还可以进行扩展
//-----------------------------------------------
BYTE Deal_698DataLenth( BYTE *buf, BYTE *Datalen, eDataLenTypeChoice eTypeChoice )
{
	TTwoByteUnion Len;
	
	if( (buf == NULL) && (eTypeChoice != eUNION_OFFSET_TYPE))
	{
		return 0;
	}
	
	if( eTypeChoice == eUNION_TYPE )//输出数据长度
	{
		memcpy( Len.b, Datalen, 2);
		if( Len.w < 0x7f )
		{
			buf[0] = Datalen[0];
			return 1;
		}
		else if( Len.w < 0xff )
		{
			buf[0] = 0x81;
			buf[1]  = Datalen[0];
			return 2;
		}
		else
		{
			buf[0] = 0x82;
			buf[1]  = Datalen[1];
			buf[2]  = Datalen[0];	

			memcpy( Len.b, Datalen, 2);//进行极限值判断 避免数组超限
			if( Len.w > MAX_APDU_SIZE )
			{
                memset( Datalen, 0x00, 2 );
			}
			
			return 3;
		}
	}
	else if( eTypeChoice == ePROTOCOL_TYPE )//输入数据长度
	{	
		if( buf[0] == 0x82 )
		{
			Datalen[0] = buf[2];
			Datalen[1] = buf[1];
			
			memcpy( Len.b, Datalen, 2);//进行极限值判断 避免数组超限
			if( Len.w > MAX_APDU_SIZE )
			{
                memset( Datalen, 0x00, 2 );
			}
			
			return 3;
		}
		else if( buf[0] == 0x81 )
		{
			Datalen[0] = buf[1];
			Datalen[1]  = 0x00 ;	
			return 2;
		}
		else
		{
			Datalen[0] = buf[0];
			Datalen[1]  = 0x00 ;	
			return 1;
		}
	}
    else if( eTypeChoice == eESAM_TYPE )
    {
    	lib_ExchangeData( Len.b, Datalen, 2);
		if( Len.w < 0x7f )
		{
			buf[0] = Datalen[0];
			return 1;
		}
		else if( Len.w < 0xff )
		{
			buf[0] = 0x81;
			buf[1]  = Datalen[0];
			return 2;
		}
		else
		{
			buf[0] = 0x82;
			buf[1]  = Datalen[1];
			buf[2]  = Datalen[0];
			
			memcpy( Len.b, Datalen, 2);//进行极限值判断 避免数组超限
			if( Len.w > MAX_APDU_SIZE )
			{
                memset( Datalen, 0x00, 2 );
			}
			
			return 3;
		}
    }
   else if( eTypeChoice == eUNION_OFFSET_TYPE )
   {
		memcpy( Len.b, Datalen, 2);

		//根据长度计算 该长度值 在698协议中的所占的字节数。
		//在调用该接口对APDU加密时，MO没有发送数据，MI却能正确接收，并且格式校验完全正确，返回长度为1，当前长度减去4字节MAC
		//导致密文长度变为负值，冲内存。
		if( Len.w > MAX_APDU_SIZE )	
		{
			memset( Datalen, 0x00, 2 );
			Len.w = 0;
		}

		if( Len.w < 0x7f )
		{ 
			return 1;
		}
		else if( Len.w < 0xff )
		{ 
			return 2;
		}
		else
		{ 
			return 3;
		}
    }
	else if (eTypeChoice == eUNION_BITSTRING_TYPE) // 输出数据长度
	{
		memcpy(Len.b, Datalen, 2);
		if (Len.w < 0x7f)
		{
			buf[0] = Datalen[0];
			return 1;
		}
		else if (Len.w < 0xff)
		{
			buf[0] = 0x81;
			buf[1] = Datalen[0];
			return 2;
		}
		else
		{
			buf[0] = 0x82;
			buf[1] = Datalen[1];
			buf[2] = Datalen[0];

			return 3;
		}
	}
	
    return 0;
}

//--------------------------------------------------
//功能描述:  添加删除安全模式参数函数
//         
//参数:     BYTE byType[in]	//byType:0--Add 1--Delete
//         
//        BYTE *pBuf[in]	//输入buf指针
//         
//返回值:    无 
//         
//备注内容:  无
//--------------------------------------------------
static void AlterSafeModePara(BYTE byType, BYTE *pBuf )//传入的缓冲从 OI_698 开始
{
	BYTE i;
	BYTE Flag;
	WORD OI,wSafeMode;
	
	if( byType > 1 )
	{
		return;
	}
	Flag = 0;	
	OI = pBuf[1]*0x100+pBuf[2];
	wSafeMode = pBuf[4]*0x100+pBuf[5];
	for( i=0; i<SafeModePara.Num; i++ )
	{
		if( OI == SafeModePara.Sub[i].OI )
		{
			if( byType == 0 )//add 如果这个OI已经存在，则覆盖safemode
			{
				if( SafeModePara.Sub[i].wSafeMode != wSafeMode )
				{
					SafeModePara.Sub[i].wSafeMode = wSafeMode; 
					Flag = 0xa5;
				}	 
			}
			else//delete
			{
				if( SafeModePara.Num != 0 )
				{
					if( i == (SafeModePara.Num-1) )//如果已是最后一个，置全FF 
					{
						memset( (void *)&SafeModePara.Sub[i], 0xff, sizeof(SafeModePara.Sub[i]) );//TSafeModeParaSub);
					}
					else//把最后一个赋过来覆盖掉
					{
						SafeModePara.Sub[i] = SafeModePara.Sub[SafeModePara.Num-1];
                        memset( (void *)&SafeModePara.Sub[SafeModePara.Num-1], 0xff, sizeof(SafeModePara.Sub[i]) );
					}
					SafeModePara.Num--;
					Flag = 0xa5;
				}	
			}
			break;
		}
	}
	if( byType == 0 )//增加的情况，且OI在 SafeModePara 中没找到
	{
		if( i >= SafeModePara.Num )
		{
			SafeModePara.Sub[SafeModePara.Num].OI = OI;
			SafeModePara.Sub[SafeModePara.Num].wSafeMode = wSafeMode;
			SafeModePara.Num++;	
			Flag = 0xa5;		
		}	
	}
	
	if( Flag == 0xa5 )
	{
		api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(ProSafeRom.SafeModePara), sizeof(TSafeModePara), (BYTE *)&SafeModePara );
	}				
}
//--------------------------------------------------
//功能描述:   清零显示安全模式参数
//         
//参数:     byType[in] byType bit0--清 SafeModePara.Flag bit1--清 SafeModePara.Num
//         
//返回值:    无 
//         
//备注内容:  无
//--------------------------------------------------
void api_ClrProSafeModePara(BYTE byType)//清零显式安全模式参数
{
	if( (byType & 0x03) == 0 )
	{
		return;
	}	
	if( byType & 0x01 )
	{
		SafeModePara.Flag = 0;//SafeModePara.Flag = 1;  @@@@@@为了测试方便将安全模式置为关闭
	}
	if( byType & 0x02 )
	{	
		SafeModePara.Num = 0;
		memset( (BYTE*)SafeModePara.Sub, 0xff, sizeof(TSafeModeParaSub)*MAX_SAFE_MODE_PARA_NUM );
	}
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(ProSafeRom.SafeModePara), sizeof(TSafeModePara), (BYTE *)&SafeModePara );	
}
//--------------------------------------------------
//功能描述:    698buf添加长度判断 仅用来添加长度
//         
//参数:      BYTE Ch[in]	     通道选择
//         
//         BYTE Mode[in]     模式选择 0X00 不能完整组帧报文模式 
//                                   0x55 可完整组帧报文模式
//                                   0x77 跟随上报可完整组帧报文模式
//         
//         BYTE Len[in]      需要添加的数据长度
//         
//返回值:     eAPPBufResultChoice buf操作结果
//         
//备注内容:    添加返回数据长度时务必使用此函数，跟随上报不预留空间需单独判断
//--------------------------------------------------
static eAPPBufResultChoice  DLT698AddBufLen(BYTE Ch, BYTE Mode,WORD Len)
{

	if( (Mode == 0x00) ||(Mode == 0x55) )//正常添加buf长度
	{
        if( ((APPData[Ch].APPCurrentBufLen.w+Len) <= APPData[Ch].APPMaxBufLen.w) && ((APPData[Ch].APPCurrentBufLen.w+Len) <= MAX_APDU_SIZE) )//小于apdu最大尺寸
        {    
            APPData[Ch].APPCurrentBufLen.w += Len;//实际组帧数据长度
        }
        else//超过最大尺寸
        {
            return eAPP_LINKDATA_OK;
        }       

		if( Mode == 0x55 )
		{
			APPData[Ch].APPBufLen.w = APPData[Ch].APPCurrentBufLen.w;
		}		
	}
	else if( Mode == 0x77 )
	{
        if( (APPData[Ch].APPCurrentBufLen.w+ Len) <= MAX_APDU_SIZE)//小于apdu最大尺寸
        {
            APPData[Ch].APPCurrentBufLen.w += Len;//实际组帧数据长度
        }
        else//超过最大尺寸
        {
            return eAPP_LINKDATA_OK;
        }
        
        APPData[Ch].APPBufLen.w = APPData[Ch].APPCurrentBufLen.w;
	}
	else
	{}

	return eADD_OK;
}
//-----------------------------------------------
//函数功能:     添加指定个数的字节到应用层buf
//
//参数:		Ch[in]       通道选择
//			Mode[in]     添加模式选择 	 0X00 不能完整组帧报文模式 
//									 0x55 可完整组帧报文模式 
//                                   0x77 用于时间标签、跟随上报的数据添加 
//			*Buf[in]     输入buf指针
//			Len[in]      长度
//返回值:	    eAPPBufResultChoice 返回添加结果
//		   
//备注:       添加返回数据时务必使用此函数，跟随上报不预留空间需单独判断
//-----------------------------------------------
static eAPPBufResultChoice  DLT698AddBuf(BYTE Ch, BYTE Mode,BYTE *Buf,WORD Len)
{
    if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return eAPP_ERR_RESPONSE;
    }

	if( (Mode == 0x00) ||(Mode == 0x55) )//正常添加buf长度
	{
		if( ((APPData[Ch].APPCurrentBufLen.w+ Len) <= APPData[Ch].APPMaxBufLen.w) && ((APPData[Ch].APPCurrentBufLen.w+Len) <= MAX_APDU_SIZE))//小于apdu最大尺寸
		{
			memcpy( &APPData[Ch].pAPPBuf[APPData[Ch].APPCurrentBufLen.w],Buf,Len);
			APPData[Ch].APPCurrentBufLen.w += Len;//实际组帧数据长度
		}
		else//超过最大尺寸
		{
			return eAPP_LINKDATA_OK;
		}
		
		if( Mode == 0x55 )
		{
			APPData[Ch].APPBufLen.w = APPData[Ch].APPCurrentBufLen.w;
		}
	}
	else if( Mode == 0x77 )
	{
        if( (APPData[Ch].APPCurrentBufLen.w+ Len) <= MAX_APDU_SIZE)//小于apdu最大尺寸
        {
            memcpy( &APPData[Ch].pAPPBuf[APPData[Ch].APPCurrentBufLen.w],Buf,Len);
            APPData[Ch].APPCurrentBufLen.w += Len;//实际组帧数据长度
        }
        else//超过最大尺寸
        {
            return eAPP_LINKDATA_OK;
        }
        
        APPData[Ch].APPBufLen.w = APPData[Ch].APPCurrentBufLen.w;
	}
	else
	{}
	
	return eADD_OK;
}
//--------------------------------------------------
//功能描述:  调用下面的函数
//         
//参数:		Ch[in]       通道选择
//			Mode[in]     添加模式选择 	 0X00 不能完整组帧报文模式 
//									 0x55 可完整组帧报文模式 
//                                   0x77 用于时间标签、跟随上报的数据添加 
//			*Buf[in]     输入buf指针
//			Len[in]      长度
//返回值:	    eAPPBufResultChoice 返回添加结果
//备注:  
//--------------------------------------------------
BYTE  api_DLT698AddBuf(BYTE Ch, BYTE Mode,BYTE *Buf,WORD Len)
{
	return DLT698AddBuf(Ch,Mode,Buf,Len);
}
//-----------------------------------------------
//函数功能:     添加1个字节到应用层buf
//
//参数:		Ch[in]    	通道选择
//			Mode[in]    添加模式选择 	0X00 不能完整组帧报文模式 
//										0x55 可完整组帧报文模式 
//										0x22 跟随上报不能完整组帧报文模式 
//										0x77 跟随上报可完整组帧报文模式
//			Data[in]   	输入的1个字节
//返回值:      eAPPBufResultChoice  返回添加结果
//		   
//备注:       添加返回数据时务必使用此函数，跟随上报不预留空间需单独判断
//-----------------------------------------------
static eAPPBufResultChoice  DLT698AddOneBuf( BYTE Ch,BYTE Mode,BYTE Data)//填充一个数据
{
    if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return eAPP_ERR_RESPONSE;
    }

	if( (Mode == 0x00) ||(Mode == 0x55) )//正常添加buf长度
	{	
		if( ((APPData[Ch].APPCurrentBufLen.w+ 1) <= APPData[Ch].APPMaxBufLen.w) && ((APPData[Ch].APPCurrentBufLen.w+1) <= MAX_APDU_SIZE))//小于apdu最大尺寸
		{
			APPData[Ch].pAPPBuf[APPData[Ch].APPCurrentBufLen.w] = Data;
			APPData[Ch].APPCurrentBufLen.w += 1;//实际组帧数据长度
		}
		else//超过最大尺寸
		{
			return eAPP_LINKDATA_OK;
		}				
		
		if( Mode == 0x55 )
		{
			APPData[Ch].APPBufLen.w = APPData[Ch].APPCurrentBufLen.w;
		}
	}
	else if( Mode == 0x77 )
	{
        if( (APPData[Ch].APPCurrentBufLen.w+ 1) <= MAX_APDU_SIZE)//小于apdu最大尺寸
        {
           APPData[Ch].pAPPBuf[APPData[Ch].APPCurrentBufLen.w] = Data;
			APPData[Ch].APPCurrentBufLen.w += 1;//实际组帧数据长度
        }
        else//超过最大尺寸
        {
            return eAPP_LINKDATA_OK;
        }
        
        APPData[Ch].APPBufLen.w = APPData[Ch].APPCurrentBufLen.w;
	}
	else
	{}
	
	return eADD_OK;
}
//--------------------------------------------------
//功能描述:    通用添加DAR方法
//         
//参数:      BYTE Ch[in]
//         
//         BYTE DAR[in]//响应数据   CHOICE  {     错误信息     [0] DAR，   M条记录     [1] SEQUENCE OF A-RecordRow}
//         
//返回值:     eAPPBufResultChoice  返回添加结果
//         
//备注内容:    无
//--------------------------------------------------
static eAPPBufResultChoice  DLT698AddBufDAR( BYTE Ch,BYTE DAR )
{
	eAPPBufResultChoice eResultChoice;

    if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return eAPP_ERR_RESPONSE;
    }

	eResultChoice = DLT698AddOneBuf(Ch,0, 0);
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	
	eResultChoice = DLT698AddOneBuf(Ch,0x55, DAR);//DAR
	return eResultChoice;
}

//-----------------------------------------------
//函数功能:     698协议中sequence of num的处理
//
//参数:       BYTE Ch[in]                     //通道		
//          WORD NoAddress[in]              //NUM所在的地址
//			BYTE *No[in]                    //实际条数
//
//返回值:	    BYTE 数据长度
//		   
//备注:       698数据长度处理，主要用来处理数据长度超过一个字节的情况 遇到优化问题,此函数还可以进行扩展
//-----------------------------------------------
void Deal_698SequenceOfNumData( BYTE Ch, WORD NoAddress, BYTE *No )
{
    BYTE LenOffest;
    TTwoByteUnion TmpNo,Len;
    
    if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return;
    }

    memcpy( TmpNo.b, No, sizeof(TTwoByteUnion));
    LenOffest = Deal_698DataLenth( NULL, TmpNo.b, eUNION_OFFSET_TYPE);

    if( LenOffest != 1 )
    {
        if( APPData[Ch].APPBufLen.w >= NoAddress)//进行极限值判断
        {
            Len.w = APPData[Ch].APPBufLen.w -NoAddress;
        }
        else if( APPData[Ch].APPCurrentBufLen.w >= NoAddress )
        {
            Len.w = APPData[Ch].APPBufLen.w -NoAddress;
        }
        else
        {
            Len.w = 0;
        }
        memmove( (BYTE*)&APPData[Ch].pAPPBuf[NoAddress+LenOffest-1],(BYTE*)&APPData[Ch].pAPPBuf[NoAddress], Len.w);
        Deal_698DataLenth( (BYTE*)&APPData[Ch].pAPPBuf[NoAddress-1], TmpNo.b, eUNION_TYPE);
        DLT698AddBufLen(Ch, 0x77, (LenOffest-1) );//记录条数buf长度改变-长度已预留直接添加即可
    }
    else
    {
        APPData[Ch].pAPPBuf[(NoAddress-1)] = TmpNo.w;
    }
}

//-----------------------------------------------
//函数功能:     用来处理SID_MAC数据
//
//参数:		
//			BYTE Mode[in]     //mode:0x00 普通的sid组帧模式  0x55 数据更新的特殊模式 需要将数据域的OAD+len 5个字节去除
//			BYTE *psmbuf[in]  //sid_mac的数据指针 包括数据长度
//			BYTE *pdata[in]   //需要组SID_MAC的数据指针，包括数据长度
//          BYTE *OutBuf[in]  //输出buf
//返回值:		WORD   返回下行数据长度
//		   
//备注:       无
//-----------------------------------------------
static WORD Deal_SIDMACDate( BYTE Mode, BYTE *psmbuf, BYTE *pdata, BYTE *OutBuf )
{
	TTwoByteUnion SMLen,DataLen,MacLen;
	BYTE SMOffest,DataOffest,MacOffest;
	
	memcpy( OutBuf, psmbuf, 04);//组前4个字节
	
	SMOffest = Deal_698DataLenth( &psmbuf[4], SMLen.b, ePROTOCOL_TYPE);
	memcpy( &OutBuf[4], &psmbuf[4+SMOffest], SMLen.w);//组后面的数据

	DataOffest = Deal_698DataLenth( pdata, DataLen.b, ePROTOCOL_TYPE);
	if( Mode == 0x55 )
	{
	    DataLen.w = DataLen.w-5;//不保存OAD+数据len
		memcpy( &OutBuf[4+SMLen.w], &pdata[DataOffest+5], DataLen.w );//组数据
	}
	else
	{
		memcpy( &OutBuf[4+SMLen.w], &pdata[DataOffest], DataLen.w);//组数据
	}


	MacOffest = Deal_698DataLenth( &psmbuf[4+SMOffest+SMLen.w], MacLen.b, ePROTOCOL_TYPE);
	memcpy( &OutBuf[4+SMLen.w+DataLen.w], &psmbuf[4+SMOffest+SMLen.w+MacOffest], MacLen.w);//组数据
		
	return ( 4+SMOffest+SMLen.w+DataOffest+DataLen.w+MacOffest+MacLen.w);
}

//-----------------------------------------------
//函数功能:     用来处理SID数据
//
//参数:		BYTE Mode[in]     0x00 代表不需要组数据 0x55代表需要组数据 //0x00针对与只有sid无后续数据的情况
//			BYTE *psmbuf[in]  sid的数据指针 包括数据长度
//			BYTE *pdata[in]   需要组SID的数据指针，包括数据长度
//			BYTE *OutBuf[in]  输出buf
//
//返回值:		WORD  返回下行数据长度
//		   
//备注:       无
//-----------------------------------------------
static WORD Deal_SIDDate(BYTE Mode,BYTE *psmbuf,BYTE *pdata,BYTE *OutBuf )
{
	TTwoByteUnion SMLen,DataLen;
	BYTE SMOffest,DataOffest;
	
	memcpy( OutBuf, psmbuf, 04);//组前4个字节
	
	SMOffest = Deal_698DataLenth( &psmbuf[4], SMLen.b, ePROTOCOL_TYPE);
	memcpy( &OutBuf[4], &psmbuf[4+SMOffest], SMLen.w);//组后面的数据

	if( Mode == 0x55)
	{
		DataOffest = Deal_698DataLenth( pdata, DataLen.b, ePROTOCOL_TYPE);
		memcpy( &OutBuf[4+SMLen.w], &pdata[DataOffest], DataLen.w);//组数据
		return ( 4+SMOffest+SMLen.w+DataOffest+DataLen.w);
	}
	return ( 4+SMOffest+SMLen.w);		
}
//--------------------------------------------------
//功能描述:  数组元素索引不为零的情况下对数据进行处理
//         
//参数:      BYTE   *pData[in] 需要判断的数据指针
//         
//返回值:     BYTE*  返回处理后的数据指针
//         
//备注内容:    无
//--------------------------------------------------
static BYTE *DealArrayClassIndexNotZeroData( BYTE *pData )
{
	pData = pData -2;//将数据指针偏移两个指针
	pData[0] = Array_698;
	pData[1] = 1;
	return pData;
}
//--------------------------------------------------
//功能描述:  结构体元素索引不为零的情况下对数据进行处理
//         
//参数:     BYTE * pData[in]
//         
//返回值:    BYTE*  返回处理后的数据指针
//         
//备注内容:  无
//--------------------------------------------------
static BYTE *DealStructureClassIndexNotZeroData( BYTE *pData )
{
	pData = pData -2;//将数据指针偏移两个指针
	pData[0] = Structure_698;
	pData[1] = 1;
	return pData;
}
//-----------------------------------------------
//函数功能:     安全请求数据处理
//
//参数:		BYTE Ch[in]   //通道选择

//返回值:		eSecurityMode 返回安全模式
//		   
//备注:       无
//-----------------------------------------------
static eSecurityMode ProcSecurityRequestData( BYTE Ch )
{
	TTwoByteUnion DataLen,Len,Len1;
	BYTE  DataOffest,SecurityMode,LenOffest,Len1Offest;
	WORD Result;
	
	if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return eSECURITY_FAIL;
    }

	//支持安全传输响应应答
	if( (LinkData[Ch].pAPP[0] != SECURITY_REQUEST) && (LinkData[Ch].pAPP[0] != SECURITY_RESPONSE) )
	{
		return eNO_SECURITY;
	}
	
	LenOffest = Deal_698DataLenth( &LinkData[Ch].pAPP[2], Len.b, ePROTOCOL_TYPE);//获取数据长度
	Len.w += LenOffest+2;

	SecurityMode =LinkData[Ch].pAPP[1];
	
	if( SecurityMode == 0xff )
	{
		return eSECURITY_FAIL;
	}
	else if( LinkData[Ch].pAPP[0] == SECURITY_REQUEST )
	{
		if( (SecurityMode == 0x00) || (SecurityMode == 0x01) )//明文方式
		{
			if( LinkData[Ch].pAPP[Len.w] == 0x00 )//判断是否选择sid_mac
			{	
				if( LinkData[Ch].AddressType != eBROADCAST_ADDRESS_MODE)//只有广播命令不判断是否建立应用链接
				{
					if( (CURR_COMM_TYPE(Ch) == COMM_TYPE_TERMINAL ) )
					{
						if( api_GetSysStatus( eSYS_STATUS_ID_698TERMINAL_AUTH ) == FALSE)//终端加密
						{
							return eSECURITY_FAIL;
						}
					}
					else
					{
						if( api_GetSysStatus( eSYS_STATUS_ID_698MASTER_AUTH ) == FALSE )//主站加密
						{
							return eSECURITY_FAIL;
						}
					}
				}
				
				Deal_SIDMACDate(0x00, &LinkData[Ch].pAPP[Len.w+1],&LinkData[Ch].pAPP[2],ProtocolBuf);
				Result = api_AuthDataSID( ProtocolBuf );
				
				if( Result == TRUE )
				{
					DataLen.b[0] = ProtocolBuf[1] ;
					DataLen.b[1] = ProtocolBuf[0] ;
					memcpy( LinkData[Ch].pAPP, &ProtocolBuf[2], DataLen.w);
					memcpy( SecurityRequestBuf, &ProtocolBuf[2], 50);//保存解密数据
					LinkData[Ch].pAPPLen.w = DataLen.w;
	
					if( SecurityMode == 0x01 )//密文模式
					{	
						return eSECRET_MAC;
					}
					else
					{
						return eEXPRESS_MAC;
					}
					
				}
				else
				{
					return eSECURITY_FAIL;
				}
			}
			else if( LinkData[Ch].pAPP[Len.w] == 0x01 )//随机数 默认随机数不会超过0xff个字节
			{
				Len1Offest = Deal_698DataLenth( &LinkData[Ch].pAPP[Len.w+1], Len1.b, ePROTOCOL_TYPE);//获取偏移长度
				if( Len1.w > MAX_RN_SIZE )//进行极限值判断
				{
					return eSECURITY_FAIL;
				}
				
				RN[0] = Len1.w;
				
				memcpy( &RN[1], &LinkData[Ch].pAPP[Len.w+1+Len1Offest], RN[0] );
				
				DataOffest = Deal_698DataLenth( &LinkData[Ch].pAPP[2], DataLen.b, ePROTOCOL_TYPE);//获取偏移长度
				LinkData[Ch].pAPP = &LinkData[Ch].pAPP[2+DataOffest ];//改变指针 到真正apdu层
	
				LinkData[Ch].pAPPLen.w = DataLen.w;//改变长度
				return eSECURITY_RN;
			}
			else if( LinkData[Ch].pAPP[Len.w] == 0x02 )//随机数+mac 未实现 只为电表主动上报实现
			{
				return eSECURITY_FAIL;
			}
			else if( LinkData[Ch].pAPP[Len.w] == 0x03 )//SID
			{
				if( LinkData[Ch].AddressType != eBROADCAST_ADDRESS_MODE)//只有广播命令不判断是否建立应用链接
				{
					if( (CURR_COMM_TYPE(Ch) == COMM_TYPE_TERMINAL ) )
					{
						if( api_GetSysStatus( eSYS_STATUS_ID_698TERMINAL_AUTH ) == FALSE)//终端加密
						{
							return eSECURITY_FAIL;
						}
					}
					else
					{
						if( api_GetSysStatus( eSYS_STATUS_ID_698MASTER_AUTH ) == FALSE )//主站加密
						{
							return eSECURITY_FAIL;
						}
					}
				}

				Deal_SIDDate(0x55,&LinkData[Ch].pAPP[Len.w+1] ,&LinkData[Ch].pAPP[2],ProtocolBuf);
				Result = api_AuthDataSID( ProtocolBuf );
				if( Result == TRUE )
				{
					if( SecurityMode == 0x01)//密文模式
					{
						DataLen.b[0] =ProtocolBuf[1] ;
						DataLen.b[1] = ProtocolBuf[0] ;
						memcpy( LinkData[Ch].pAPP, &ProtocolBuf[2], DataLen.w);
						memcpy( SecurityRequestBuf, &ProtocolBuf[2], 50);//保存解密数据
						LinkData[Ch].pAPPLen.w = DataLen.w;
						return eSECRET_TEXT;
					}
					else
					{
						return eSECURITY_FAIL;//不存在明文+sid的情况
					}
					
				}
				else
				{
					return eSECURITY_FAIL;
				}
			}
		}
		else
		{
			return eSECURITY_FAIL;
		}
	}
	else
	{
		if( SecurityMode == 0xff )
		{
			return eSECURITY_FAIL;
		}
		if( LinkData[Ch].pAPP[0] == SECURITY_RESPONSE )
		{
			if( SecurityMode == 0x00 )//明文方式
			{
				if( LinkData[Ch].pAPP[Len.w] != 0x00 )//OPTIONAL 选择
				{
					if( LinkData[Ch].pAPP[Len.w+1] == 0x00 )//选择MAC
					{
						LenOffest = Deal_698DataLenth( &LinkData[Ch].pAPP[2], Len.b, ePROTOCOL_TYPE);//获取偏移长度
						if( Len.w > MAX_APDU_SIZE )//进行极限值判断
						{
							return eSECURITY_FAIL;
						}

						memcpy( ProtocolBuf, &LinkData[Ch].pAPP[2+LenOffest],Len.w );
						
						Len1Offest = Deal_698DataLenth( &LinkData[Ch].pAPP[2+LenOffest+Len.w+2], Len1.b, ePROTOCOL_TYPE);//获取偏移长度
						if( Len1.w > MAX_RN_SIZE )//进行极限值判断
						{
							return eSECURITY_FAIL;
						}
						
						memcpy( ProtocolBuf+Len.w, &LinkData[Ch].pAPP[2+LenOffest+Len.w+2+Len1Offest],Len1.w );
					
						Result = api_VerifyEsamSafeReportMAC( Len1.w+Len.w, ProtocolBuf , RN_MAC);
						if( Result == TRUE )
						{
							memcpy( LinkData[Ch].pAPP, ProtocolBuf, Len.w);
							LinkData[Ch].pAPPLen.w = Len.w;
							return eRNMAC;
						}
						else
						{
							//使用备份进行确认
							Result = api_VerifyEsamSafeReportMAC( Len1.w+Len.w, ProtocolBuf , RN_MAC_BAK);
							if( Result == TRUE )
							{
								memcpy( LinkData[Ch].pAPP, ProtocolBuf, Len.w);
								LinkData[Ch].pAPPLen.w = Len.w;
								return eRNMAC;
							}
							
							return eSECURITY_FAIL;
						}
	
					}
				}
			}
		}
	}
	
	return eSECURITY_FAIL;
}

//--------------------------------------------------
//功能描述: 用于终端分支的数据解密
//
//参数:		APDU[in]: 解密
//			CommType[in]: 终端主站
//			Out[out]:
//返回值: 当前的加密方式
//
//备注:
//--------------------------------------------------
BYTE TremainProcSecurityRequestData( BYTE *APDU, BYTE CommType, BYTE *Out ,WORD* wDataLen)
{
	TTwoByteUnion DataLen, Len, Len1;
	BYTE DataOffest, SecurityMode, LenOffest, Len1Offest;
	WORD Result;

	//支持安全传输响应应答
	if( (APDU[0] != SECURITY_REQUEST) && (APDU[0] != SECURITY_RESPONSE) )
	{
		return eNO_SECURITY;
	}
	
	LenOffest = Deal_698DataLenth( &APDU[2], Len.b, ePROTOCOL_TYPE);//获取数据长度
	Len.w += LenOffest + 2;

	SecurityMode = APDU[1];
	
	if( SecurityMode == 0xff )
	{
		return eSECURITY_FAIL;
	}
	else if( APDU[0] == SECURITY_REQUEST )
	{
		if( (SecurityMode == 0x00) || (SecurityMode == 0x01) )//明文方式
		{
			if( APDU[Len.w] == 0x00 )//判断是否选择sid_mac
			{
				if( CommType == COMM_TYPE_TERMINAL )
				{
					if( api_GetSysStatus( eSYS_STATUS_ID_698TERMINAL_AUTH ) == FALSE)//终端加密
					{
						return eSECURITY_FAIL;
					}
				}
				else if( api_GetSysStatus( eSYS_STATUS_ID_698MASTER_AUTH ) == FALSE )//主站加密
				{
					return eSECURITY_FAIL;
				}
				
				Deal_SIDMACDate(0x00, &APDU[Len.w + 1], &APDU[2], ProtocolBuf);
				Result = api_AuthDataSID( ProtocolBuf );
				if( Result == TRUE )
				{
					DataLen.b[0] = ProtocolBuf[1] ;
					DataLen.b[1] = ProtocolBuf[0] ;
					memcpy( Out, &ProtocolBuf[2], DataLen.w);
					memcpy( SecurityRequestBuf, &ProtocolBuf[2], 50);//保存解密数据
					
					*wDataLen = DataLen.w;
	
					if( SecurityMode == 0x01 )//密文模式
					{	
						return eSECRET_MAC;
					}
					else
					{
						return eEXPRESS_MAC;
					}
				}
			}
			else if( APDU[Len.w] == 0x01 )//随机数 默认随机数不会超过0xff个字节
			{
				Len1Offest = Deal_698DataLenth( &APDU[Len.w+1], Len1.b, ePROTOCOL_TYPE);//获取偏移长度
				if( Len1.w > MAX_RN_SIZE )//进行极限值判断
				{
					return eSECURITY_FAIL;
				}
				
				RN[0] = Len1.w;
				memcpy( &RN[1], &APDU[Len.w+1+Len1Offest], RN[0] );
				
				DataOffest = Deal_698DataLenth( &APDU[2], DataLen.b, ePROTOCOL_TYPE);//获取偏移长度
				memmove(Out, &APDU[2 + DataOffest], DataLen.w);

				*wDataLen = DataLen.w;

				return eSECURITY_RN;
			}
			else if( APDU[Len.w] == 0x02 )//随机数+mac 未实现 只为电表主动上报实现
			{
				return eSECURITY_FAIL;
			}
			else if( APDU[Len.w] == 0x03 )//SID
			{
				if( CommType == COMM_TYPE_TERMINAL )
				{
					if( api_GetSysStatus( eSYS_STATUS_ID_698TERMINAL_AUTH ) == FALSE)//终端加密
					{
						return eSECURITY_FAIL;
					}
				}
				else if( api_GetSysStatus( eSYS_STATUS_ID_698MASTER_AUTH ) == FALSE )//主站加密
				{
					return eSECURITY_FAIL;
				}

				Deal_SIDDate(0x55, &APDU[Len.w + 1], &APDU[2], ProtocolBuf);
				Result = api_AuthDataSID( ProtocolBuf );
				if( Result == TRUE )
				{
					if( SecurityMode == 0x01)//密文模式
					{
						DataLen.b[0] =ProtocolBuf[1] ;
						DataLen.b[1] = ProtocolBuf[0] ;
						memcpy( Out, &ProtocolBuf[2], DataLen.w);
						memcpy( SecurityRequestBuf, &ProtocolBuf[2], 50);//保存解密数据
						*wDataLen = DataLen.w;

						return eSECRET_TEXT;
					}	
				}
			}
		}
	}
	
	return eSECURITY_FAIL;
}

//--------------------------------------------------
//功能描述: 数据加密
//
//参数:	SafeMode[in]: TremainProcSecurityRequestData()返回值
//		APDU[in/out]: 待加密的数据
//		CommType[in]: 终端 主站
//		APDULen[in]: 加密数据的长度
//返回值: 0x8000长度超限， 密文长度
//
//备注:无
//--------------------------------------------------
WORD TremainProcSecurityResponseData ( BYTE SafeMode, BYTE *APDU, BYTE CommType, WORD APDULen)
{
	TTwoByteUnion Len;
	BYTE BufIndex, *Respon = APDU;
	WORD Result = FALSE;
    
	if(SafeMode == eNO_SECURITY)	//明文不加密
	{
		return APDULen;
	}

	if( SafeMode != eSECURITY_FAIL )
	{
		if( ( SafeMode == eSECRET_MAC ) ||( SafeMode == eSECRET_TEXT )||(SafeMode == eEXPRESS_MAC ))//P2：11：明文+MAC 12：密文 13： 密文+MAC
		{
			if( APDULen > sizeof(ProtocolBuf) )//数据长度溢出返回
			{
				return 0x8000;
			}
			memcpy(ProtocolBuf, APDU, APDULen);
			memcpy( SecurityResponseBuf, APDU, 50);//保存解密数据
			if( CommType == COMM_TYPE_TERMINAL )//终端加密
			{
				Result = api_PackEsamDataWithSafe(0x55, (SafeMode - 3), APDULen, ProtocolBuf);
			}
			else//主站加密
			{
				Result = api_PackEsamDataWithSafe(0x00, (SafeMode - 3), APDULen, ProtocolBuf);
			}
			
			if( Result == TRUE )
			{
				Len.b[0] = ProtocolBuf[1];
				Len.b[1] = ProtocolBuf[0];
		
				if( (SafeMode == eSECRET_MAC) ||(SafeMode == eEXPRESS_MAC)  ) 
				{
					Len.w = (Len.w-4);
					BufIndex = Deal_698DataLenth( (BYTE*)NULL, Len.b,eUNION_OFFSET_TYPE);//获取明密文数据的偏移长度
					BufIndex = BufIndex+2;
					
					memcpy( &Respon[BufIndex], &ProtocolBuf[2], Len.w );
					Respon[ Len.w+BufIndex ] = 0x01;			//有数据验证信息
					Respon[ Len.w+BufIndex+1 ] = 0x00;			//选择MAC
					Respon[ Len.w+BufIndex+1+1 ] = 0x04;		//MAC 长4字节
					memcpy( &Respon[ Len.w+BufIndex+1+1+1],&ProtocolBuf[2+Len.w],0x04);	//4字节MAC

					Respon[0] = SECURITY_RESPONSE;//组安全传输命令头
					if( SafeMode == eSECRET_MAC )//选择密文
					{
						Respon[1] = 0x01;
					}
					else//选择明文
					{
						Respon[1] = 0x00;
					}
			
					Deal_698DataLenth( &Respon[2], Len.b,eUNION_TYPE);	//密文长度，不包括MAC
					APDULen = (Len.w+BufIndex+7);//+安全传输头+mac
				}
				else	//下发 密文 + SID ，返回密文  
				{
					BufIndex = Deal_698DataLenth( (BYTE*)NULL, Len.b,eUNION_OFFSET_TYPE);
					BufIndex = BufIndex+2;
					
					memcpy( &Respon[BufIndex], &ProtocolBuf[2], Len.w);//搬运数据
					Respon[ Len.w+BufIndex ] = 0x00;//optional 不选择数据
					
					Respon[0] =SECURITY_RESPONSE;//组安全传输命令头
					Respon[1] = 0x01;
					Deal_698DataLenth( &Respon[2], Len.b,eUNION_TYPE);
					APDULen = (Len.w+BufIndex+1);//+安全传输头+mac
				}
			}
		}
		else if( SafeMode == eSECURITY_RN )
		{
			memcpy(&ProtocolBuf, &RN[1], RN[0]);
			memcpy(&ProtocolBuf[RN[0]], APDU, APDULen);
			Result = api_PackDataWithSafe((APDULen + RN[0]), ProtocolBuf);
			if( Result == TRUE )
			{				
				BufIndex = Deal_698DataLenth( (BYTE*)NULL, (BYTE *)&APDULen,eUNION_OFFSET_TYPE);//获取明密文数据的偏移长度
				BufIndex = BufIndex+2;

				memmove(&Respon[BufIndex], APDU, APDULen);
				Respon[ APDULen+BufIndex ] = 0x01;
               	Respon[ APDULen+BufIndex+1 ] = 0x00;
                Respon[ APDULen+BufIndex+1+1 ] = 0x04;
                memcpy( &Respon[ APDULen+BufIndex+1+1+1],&ProtocolBuf[2],4);

                Respon[0] =SECURITY_RESPONSE;//组安全传输命令头
                Respon[1] = 0x00;
                               
                Deal_698DataLenth( &Respon[2],(BYTE *)&APDULen,eUNION_TYPE);
                APDULen = (4+APDULen+BufIndex+3);//+安全传输头+mac
			}
		}
	}

	if(Result == FALSE)
	{
		Respon[0] =SECURITY_RESPONSE;
		Respon[1] = 0x02;
		Respon[2] = DAR_Symmetry;//DAR
		Respon[3] = 00;
		APDULen = 4;//+安全传输头+mac
	}

	return APDULen;
}

//-----------------------------------------------
//函数功能:     安全响应数据处理
//
//参数:		BYTE Ch[in]   //通道选择
//
//返回值:		无
//		   
//备注:       无
//-----------------------------------------------
static void ProcSecurityResponseData ( BYTE Ch)
{
	TTwoByteUnion Len;
	BYTE BufIndex;
	WORD Result;
	
	if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return;
    }
    
	if( (APPData[Ch].eSafeMode != eSECURITY_FAIL) && (APPData[Ch].eSafeMode != eNO_SECURITY)  )
	{
		if( ( APPData[Ch].eSafeMode == eSECRET_MAC ) ||( APPData[Ch].eSafeMode == eSECRET_TEXT )||( APPData[Ch].eSafeMode == eEXPRESS_MAC ))//P2：11：明文+MAC 12：密文 13： 密文+MAC
		{
			if( APPData[Ch].APPBufLen.w > sizeof(ProtocolBuf) )//数据长度溢出返回
			{
				return;
			}
			memcpy( ProtocolBuf,APPData[Ch].pAPPBuf,APPData[Ch].APPBufLen.w);//进行数据的搬运
			memcpy( SecurityResponseBuf, APPData[Ch].pAPPBuf, 50);//保存解密数据
			if( CURR_COMM_TYPE(Ch) == COMM_TYPE_TERMINAL )//终端加密
			{
				Result = api_PackEsamDataWithSafe( 0x55, (APPData[Ch].eSafeMode-3),APPData[Ch].APPBufLen.w,ProtocolBuf);
			}
			else//主站加密
			{
				Result = api_PackEsamDataWithSafe( 0x00, (APPData[Ch].eSafeMode-3),APPData[Ch].APPBufLen.w,ProtocolBuf);
			}
			
			if( Result == TRUE )
			{
				Len.b[0] = ProtocolBuf[1];
				Len.b[1] = ProtocolBuf[0];
		
				if( (APPData[Ch].eSafeMode == eSECRET_MAC) ||(APPData[Ch].eSafeMode == eEXPRESS_MAC)  ) 
				{
					Len.w = (Len.w-4);
					BufIndex = Deal_698DataLenth( (BYTE*)NULL, Len.b,eUNION_OFFSET_TYPE);//获取明密文数据的偏移长度
					BufIndex = BufIndex+2;
					
					memcpy( &APPData[Ch].pAPPBuf[BufIndex], &ProtocolBuf[2], Len.w );
					APPData[Ch].pAPPBuf[ Len.w+BufIndex ] = 0x01;
					APPData[Ch].pAPPBuf[ Len.w+BufIndex+1 ] = 0x00;
					APPData[Ch].pAPPBuf[ Len.w+BufIndex+1+1 ] = 0x04;
					memcpy( &APPData[Ch].pAPPBuf[ Len.w+BufIndex+1+1+1],&ProtocolBuf[2+Len.w],0x04);

					APPData[Ch].pAPPBuf[0] =SECURITY_RESPONSE;//组安全传输命令头
					if( APPData[Ch].eSafeMode == eSECRET_MAC )//选择密文
					{
						APPData[Ch].pAPPBuf[1] = 0x01;
					}
					else//选择明文
					{
						APPData[Ch].pAPPBuf[1] = 0x00;
					}
			
					Deal_698DataLenth( &APPData[Ch].pAPPBuf[2], Len.b,eUNION_TYPE);
					APPData[Ch].APPBufLen.w = (Len.w+BufIndex+7);//+安全传输头+mac
					APPData[Ch].APPCurrentBufLen.w = APPData[Ch].APPBufLen.w;
				}
				else
				{
					BufIndex = Deal_698DataLenth( (BYTE*)NULL, Len.b,eUNION_OFFSET_TYPE);
					BufIndex = BufIndex+2;
					
					memcpy( &APPData[Ch].pAPPBuf[BufIndex], &ProtocolBuf[2], Len.w);//搬运数据
					APPData[Ch].pAPPBuf[ Len.w+BufIndex ] = 0x00;//optional 不选择数据
					
					APPData[Ch].pAPPBuf[0] =SECURITY_RESPONSE;//组安全传输命令头
					APPData[Ch].pAPPBuf[1] = 0x01;
					Deal_698DataLenth( &APPData[Ch].pAPPBuf[2], Len.b,eUNION_TYPE);
					APPData[Ch].APPBufLen.w = (Len.w+BufIndex+1);//+安全传输头+mac
					APPData[Ch].APPCurrentBufLen.w = APPData[Ch].APPBufLen.w;
				}
			}
			else//返回失败
			{
				APPData[Ch].pAPPBuf[0] =SECURITY_RESPONSE;
				APPData[Ch].pAPPBuf[1] = 0x02;
				APPData[Ch].pAPPBuf[2] = DAR_Symmetry;//DAR
				APPData[Ch].pAPPBuf[3] = 00;
				APPData[Ch].APPBufLen.w = 4;//+安全传输头+mac
				APPData[Ch].APPCurrentBufLen.w = APPData[Ch].APPBufLen.w;
			}
		}
		else if( APPData[Ch].eSafeMode == eSECURITY_RN )
		{
			memcpy( &ProtocolBuf, &RN[1],RN[0]);
			memcpy( &ProtocolBuf[RN[0]], APPData[Ch].pAPPBuf,APPData[Ch].APPBufLen.w);
			if(api_PackDataWithSafe((APPData[Ch].APPBufLen.w+RN[0]),ProtocolBuf) == FALSE)
			{
				memset(ProtocolBuf, 0xFF, 6);		//ESAM 通信失败，MAC返回全FF，不返回对称解密错误，防止现场数据抄读不回来。
			}
				
			BufIndex = Deal_698DataLenth( (BYTE*)NULL, APPData[Ch].APPBufLen.b,eUNION_OFFSET_TYPE);//获取明密文数据的偏移长度
			BufIndex = BufIndex+2;
							
			memmove( &APPData[Ch].pAPPBuf[BufIndex],APPData[Ch].pAPPBuf,APPData[Ch].APPBufLen.w);
			APPData[Ch].pAPPBuf[ APPData[Ch].APPBufLen.w+BufIndex ] = 0x01;
			APPData[Ch].pAPPBuf[ APPData[Ch].APPBufLen.w+BufIndex+1 ] = 0x00;
			APPData[Ch].pAPPBuf[ APPData[Ch].APPBufLen.w+BufIndex+1+1 ] = 0x04;

			memcpy(&APPData[Ch].pAPPBuf[APPData[Ch].APPBufLen.w + BufIndex + 1 + 1 + 1], &ProtocolBuf[2], 4);

			APPData[Ch].pAPPBuf[0] =SECURITY_RESPONSE;//组安全传输命令头
			APPData[Ch].pAPPBuf[1] = 0x00;
							
			Deal_698DataLenth( &APPData[Ch].pAPPBuf[2],APPData[Ch].APPBufLen.b,eUNION_TYPE);
			APPData[Ch].APPBufLen.w = (4+APPData[Ch].APPBufLen.w+BufIndex+3);//+安全传输头+mac
			APPData[Ch].APPCurrentBufLen.w = APPData[Ch].APPBufLen.w;
		}
	}
	else if( APPData[Ch].eSafeMode == eSECURITY_FAIL )
	{
		if( LinkData[Ch].pAPP[0] == SECURITY_RESPONSE )//如果响应解密失败不返回数据
		{
			APPData[Ch].APPFlag = APP_NO_DATA;
		}
		else
		{
			APPData[Ch].APPFlag = APP_HAVE_DATA;
			APPData[Ch].pAPPBuf[0] =SECURITY_RESPONSE;
			APPData[Ch].pAPPBuf[1] = 0x02;
			APPData[Ch].pAPPBuf[2] = DAR_Symmetry;//DAR
			APPData[Ch].pAPPBuf[3] = 00;
			APPData[Ch].APPBufLen.w = 4;//+安全传输头+mac
			APPData[Ch].APPCurrentBufLen.w = APPData[Ch].APPBufLen.w;	
		}
	}
	
}
//-----------------------------------------------
//函数功能:    分帧判断进程
//
//参数:		BYTE Ch[in]   //通道选择
//
//返回值:		无
//		   
//备注:       无
//-----------------------------------------------
static void ProFollowJudge ( BYTE Ch)
{
    DWORD ConsultLen;

    ConsultLen = GetConsultBufLen( Ch );//获取当前通道协商长度
    
	if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return;
    }

    if( (APPData[Ch].APPBufLen.w+LinkData[Ch].AddressLen+11) > ConsultLen )//判断是否需要启用分帧 + 帧头帧尾
    {
        APPData[Ch].APPFlag |= APP_LINK_FRAME;
    }
}

//--------------------------------------------------
//功能描述:  建立应用链接认证处理
//         
//参数:    	Mode[in] 0x55: 终端 其他：主站
//			BYTE *pBuf[in]
//         
//返回值:	 	TRUE/FALSE
//         
//备注内容:  无
//--------------------------------------------------
static WORD SecurityMechanismInfo( eConnectMode* ConnectModeMode, BYTE *pBuf )
{
	BYTE result;
	BYTE LenOffset,Len1Offest;
	TTwoByteUnion Len,Len1;
	if( pBuf[0] == 0 )
	{
	    ConnectMode = 0;//保存建立应用连接的模式
	    *ConnectModeMode = eConnectGeneral;//建立一般应用链接
		return TRUE;//允许建立公共连接
	}	
	else if( pBuf[0] == 1 )
	{
		if( pBuf[1] != 8 )//一般密码明文长度 密码长度不能大于7
        {
			return FALSE;
        }  
        
        ConnectMode = 1;//保存建立应用连接的模式
		*ConnectModeMode = eConnectGeneral;//建立一般应用链接

		return Judge698PassWord( (BYTE*)&pBuf[2] );
	}
	else if( pBuf[0] == 2 )//对称加密    [2] SymmetrySecurity，
	{

		if( (APPConnect.ConnectInfo[eConnectMaster].ConnectValidTime == 0) //如果主站、终端都没建立应用链接进行复位
		&& (APPConnect.ConnectInfo[eConnectTerminal].ConnectValidTime == 0) )//其他情况判断过于复杂不进行复位
		{
	   		api_ResetEsamSpi();//调用加密前复位esam
		}

	    LenOffset = Deal_698DataLenth( (BYTE*)&pBuf[1], Len.b, ePROTOCOL_TYPE );
	    Len1Offest = Deal_698DataLenth( (BYTE*)&pBuf[1+LenOffset+Len.w], Len1.b, ePROTOCOL_TYPE );
        if( Len.w > sizeof(ProtocolBuf) )//防止数组越界
        {
        	return FALSE;
        }
        memcpy( ProtocolBuf, &pBuf[LenOffset+1],Len.w );// 密文1        octet-string，
        if( Len1.w > ( sizeof(ProtocolBuf) - Len.w ) )//防止数组越界
        {
        	return FALSE;
        }
        memcpy( &ProtocolBuf[Len.w], &pBuf[(1+LenOffset+Len.w+Len1Offest)], Len1.w );// 密文1        octet-string，

		result = api_ConnectMechanismInfo( *ConnectModeMode, ProtocolBuf );//*ConnectModeMode取指针内容 进行应用连接
		ConnectMode = 2;//保存建立应用连接的模式
		//调用函数
		//协商连接成功，则进行如下操作
		return result;//TRUE; 
	}
	return FALSE;//FALSE;//无这种方式		
}

//--------------------------------------------------
//功能描述:    698连接请求
//参数:      BYTE Ch[in]          输入通道
//         
//         BYTE *pAPDU[in]      apdu层buf指针
//         
//         BYTE *pBuf[in]       响应buf
//         
//返回值:     无
//         
//备注内容:    无
//--------------------------------------------------
static void DealConnect_Request(BYTE Ch,BYTE *pAPDU,BYTE *pBuf)
{
	WORD Result,EsamLen;
	DWORD dwData;
	BYTE *pbakbuf;
	pbakbuf = pBuf;
	TTwoByteUnion Len,BufLen;
	TFourByteUnion TmpTime,TmpTime1;
	BYTE Buf[20];
	eConnectMode eConnectMode;
	
    if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return;
    }

	APPData[Ch].APPFlag = APP_HAVE_DATA;
	EsamLen = 0x8000;

	if( CURR_COMM_TYPE(Ch) == COMM_TYPE_TERMINAL )//终端建立应用链接-一般密码连接会在SecurityMechanismInfo中进行判断-jwh
	{
		eConnectMode = eConnectTerminal;

	}
	else
	{
		eConnectMode = eConnectMaster;
	}
	
	memcpy( (void *)pBuf, (void *)pAPDU, 2);
	pBuf += 2;
	
	lib_ExchangeData(BufLen.b,pAPDU+30, 2);//取客户机接受帧最大尺寸
    ConsultData.channel = Ch;
    ConsultData.Len = BufLen.w;
    
    if( ConsultData.Len >= MAX_PRO_BUF )//超过物理空间长度使用最大长度
    {
        ConsultData.Len = MAX_PRO_BUF;
    }
    else
    {
        if( ConsultData.Len < MIN_CONSULT_BUF_LEN )//认为通道协商长度不能低于100，低于100使用默认长度
        {
            ConsultData.Len = GetDefaultConsultBufLen(Ch);
        }
    }
    
	lib_ExchangeData(TmpTime.b,pAPDU+35, 4);//取有效时间
	JTOverTime=TmpTime.d;
	//if( Ch == eIR )// 红外只有在认证请求通过后 方可建立应用链接
	//{
    //    if( api_GetSysStatus( eSYS_IR_ALLOW_PRG ) == TRUE )
    //    {
    //        Result = SecurityMechanismInfo( &eConnectMode , pAPDU+39 );//1+2+8+16+2+2+1+2+4   
    //    }
    //    else
    //    {	
    //        Result = FALSE;
    //    }
    //}
	//else
	{
        Result = SecurityMechanismInfo( &eConnectMode, pAPDU+39 );//1+2+8+16+2+2+1+2+4   
	}
	/*	
	FactoryVersion∷=SEQUENCE
	{
	厂商代码         visible-string(SIZE (4))，
	软件版本号      visible-string(SIZE (4))，
	软件版本日期   visible-string(SIZE (6))，
	硬件版本号      visible-string(SIZE (4))，
	硬件版本日期   visible-string(SIZE (6))，
	厂家扩展信息   visible-string(SIZE (8))
	}
	*/
	dwData = 0x00030043;	//暂存OAD 43000300
	//memset( (void *)pBuf, 0x00, 32);//服务器厂商版本信息
	api_GetProOadData( eGetNormalData, eData, 0xff, (BYTE *)&dwData, NULL, (4+4+6+4+6+8+4), pBuf );
	pBuf += 32;
	
	//pBuf[0] = pAPDU[2];//协议版本号返回主站下发来的
	//pBuf[1] = pAPDU[3];
	//福建局要求我们返回表自己的协议版本号，复旦微也返回电表设置的
	api_ProcMeterTypePara( READ, eMeterProtocolVersion, pBuf );//698.45协议版本号(数据类型:WORD)
	lib_InverseData( pBuf, 2 );
	pBuf += 2;
	
	memcpy( (void *)pBuf, (void *)&(pAPDU[4]), 24 );//协议一致性与功能一致性返回主站发来的 wlk
	pBuf += 24;
	//pBuf += AddProtocolAndFunctionConformance( 0, pBuf );//添加协议一致性与功能一致性Buf
	
	Len.w= ConsultData.Len;//服务器发送帧最大尺寸    long-unsigned
	pBuf[0] = Len.b[1];//应用层先传高字节
	pBuf[1] = Len.b[0];
	pBuf += 2;
	
	Len.w = MAX_PRO_BUF;//服务器接收帧最大尺寸    long-unsigned
	pBuf[0] = Len.b[1];//应用层先传高字节
	pBuf[1] = Len.b[0];
	pBuf += 2;
	
	*(pBuf++) = 1;	//服务器最大尺寸帧个数    unsigned
	
	Len.w = MAX_APDU_SIZE;//服务器最大可处理apdu最大尺寸    long-unsigned
	pBuf[0] = Len.b[1];//应用层先传高字节
	pBuf[1] = Len.b[0];
	pBuf += 2;
	
	//商定的应用连接超时时间 double-long-unsigned，
	if( pAPDU[39] == 0 )//公共连接
	{
		memcpy( (void *)pBuf,(void *)(pAPDU+35), 4);//商定的应用连接超时时间 double-long-unsigned，取 Connect-Request 中的 期望的应用连接超时时间 double-long-unsigned
	}	
	else//其它
	{	
		if( eConnectMode == eConnectTerminal )
		{
			EsamLen = api_GetEsamInfo( 0x14, Buf ); //读f1000500 ESAM对象的属性5．会话时效门限
		}
		else
		{
			EsamLen = api_GetEsamInfo( 5, Buf ); //读f1000500 ESAM对象的属性5．会话时效门限
		}
	}

	//安全认证响应对象        SecurityResponseInfo	
	//*(pBuf++) = Result; //应用连接请求认证结果
	if( LinkData[Ch].AddressType!=eSINGLE_ADDRESS_TYPE )
	{
		memcpy( (void *)pBuf, (void *)(pAPDU+35), 4 );//添加会话应用连接超时时间
	    pBuf += 4;	
        *(pBuf++) = 0xff;//认证结果      ConnectResult 中 对称解密错误         （2），
        *(pBuf++) = 0;//其他错误（时间标签错误）
	}	
	else if( ConnectMode == 0 )//建立公共应用链接
	{
        memcpy( (void *)pBuf, (void *)(pAPDU+35), 4 );//添加会话应用连接超时时间
        pBuf += 4;  
        *(pBuf++) = 0;//认证结果      ConnectResult 中 对称解密错误         （2），
        *(pBuf++) = 0;
	}
	else if( (Result == FALSE) || ( APPData[Ch].TimeTagFlag == eTime_Invalid) )
	{
	    memcpy( (void *)pBuf, (void *)(pAPDU+35), 4 );//添加会话应用连接超时时间
	    pBuf += 4;	

	    if( Result == FALSE )
	    {
            if(pAPDU[39] == 2 )//对称加密    [2] SymmetrySecurity，
            {
                *(pBuf++) = 02;//认证结果      ConnectResult 中 对称解密错误         （2），
                *(pBuf++) = 0;
            }
            else
            {
                *(pBuf++) = 01;//认证结果      ConnectResult 中 密码错误             （1），
                *(pBuf++) = 0;
            }
	    }
	    else
	    {
            *(pBuf++) = 0xff;//认证结果      ConnectResult 中 对称解密错误         （2），
            *(pBuf++) = 0;//其他错误（时间标签错误）
	    }
	}
	else
	{	
		#if( SEL_DLT645_2007 == YES )
		api_Release645_Auth();//断开645身份认证-一般密码建立应用连接也会断开
		#endif
		api_SaveProgramRecord( EVENT_END, 0x51, (BYTE *)NULL);
		
		if( eConnectMode > eConnectModeMaxNum )//极限值判断 默认为主站
		{
			eConnectMode = eConnectMaster;

		}
		
		if( EsamLen == 0x8000 ) //获取建立应用链接有效时间
		{
			APPConnect.ConnectInfo[eConnectMode].ConnectValidTime = TmpTime.d;
		}
		else
		{
			lib_ExchangeData(TmpTime1.b, Buf, 4);
			TmpTime1.d = TmpTime1.d*60;//单位分钟,转化为秒
			if( TmpTime.d > TmpTime1.d )
			{
				APPConnect.ConnectInfo[eConnectMode].ConnectValidTime = TmpTime1.d;
			}
			else
			{
				APPConnect.ConnectInfo[eConnectMode].ConnectValidTime = TmpTime.d;
			}
		}
		
        if( eConnectMode != eConnectGeneral )//不是一般密码 清零一般密码认证时间--其他模式可共存
        {
            APPConnect.ConnectInfo[eConnectGeneral].ConnectValidTime = 0;
            APPConnect.ConnectInfo[eConnectGeneral].ConstConnectValidTime = 0;

        }
        
		if( eConnectMode == eConnectGeneral )
		{
			api_SetSysStatus(eSYS_STATUS_ID_698GENERAL_AUTH);
		}
		else if( eConnectMode == eConnectTerminal )
		{
			api_SetSysStatus(eSYS_STATUS_ID_698TERMINAL_AUTH);
			api_ClrSysStatus( eSYS_TERMINAL_ALLOW_PRG );		//20规范要求二者存其一，
			APPConnect.TerminalAuthTime = 1;//1s后关闭终端认证
		}
		else
		{
			api_SetSysStatus(eSYS_STATUS_ID_698MASTER_AUTH);
		}

		ClientAddress = LinkData[Ch].ClientAddress;
		APPConnect.ConnectInfo[eConnectMode].ConstConnectValidTime = APPConnect.ConnectInfo[eConnectMode].ConnectValidTime ;
		
		lib_ExchangeData( pBuf, (BYTE*)&APPConnect.ConnectInfo[eConnectMode].ConstConnectValidTime, 4 );//添加会话应用连接超时时间
	    pBuf += 4;
	    
		*(pBuf++) = 0;// ConnectResult∷=ENUMERATED {  允许建立应用连接     （0），
		if(pAPDU[39] == 2 )//对称密码 //对称加密    [2] SymmetrySecurity，
		{
			*(pBuf++) = 0x01;//选择数据
			lib_ExchangeData(Len.b, ProtocolBuf, 2 );
			if( (Len.w > 0xff) || (Len.w < 4) )//极限值防护
			{
				Len.w = 52;
			}
			
			*(pBuf++) = (Len.w-4);//RN长度
			memcpy( (void *)pBuf, &ProtocolBuf[2],Len.w-4);//上报esam认证数据 data
			pBuf += (Len.w-4);

			*(pBuf++) = 4;//签名长度
			memcpy( (void *)pBuf, &ProtocolBuf[2+(Len.w-4)],4);//上报MAC 
			pBuf += 4;
		}
		else
		{
		    //api_ResetEsamSpi( );//一般密码复位一下esam临时密钥被恢复掉
			*(pBuf++) = 0;//非对称加密没有“认证附加信息  SecurityData  OPTIONAL”
			//宣贯材料说一般密码保留以后用，现在不用
			//APPConnect.ConnectValidTime = TmpTime.d;
		}		
	}

	APPData[Ch].APPBufLen.w = (WORD)(pBuf - pbakbuf);
    APPData[Ch].APPCurrentBufLen.w = APPData[Ch].APPBufLen.w;
	APPData[Ch].APPFlag = APP_HAVE_DATA;
	APPData[Ch].BufDealLen.w = LinkData[Ch].pAPPLen.w;
}
//--------------------------------------------------
//功能描述:  698连接断开请求
//参数:     BYTE Ch[in]
//         
//返回值:    void无返回值 
//         
//备注内容:  无
//--------------------------------------------------
static void DealRelease_Request(BYTE Ch)
{
    if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return;
    }
    
	DLT698AddBuf(Ch, 0, LinkData[Ch].pAPP,2);					//添加命令头
	DLT698AddOneBuf(Ch,0x55,0 );							    //添加结果 成功
	
	if( CURR_COMM_TYPE(Ch) ==  COMM_TYPE_TERMINAL )
	{
		api_ClrSysStatus( eSYS_STATUS_ID_698TERMINAL_AUTH );
		APPConnect.ConnectInfo[eConnectTerminal].ConnectValidTime = 1;
		APPConnect.ConnectInfo[eConnectTerminal].ConstConnectValidTime = 1;//1s后关闭终端协商
	}
	else
	{
		api_ClrSysStatus( eSYS_STATUS_ID_698MASTER_AUTH );
		APPConnect.ConnectInfo[eConnectMaster].ConnectValidTime = 1;
		APPConnect.ConnectInfo[eConnectMaster].ConstConnectValidTime = 1;//1s后关闭终端协商
	}

	api_ClrSysStatus( eSYS_STATUS_ID_698GENERAL_AUTH );
	
	APPConnect.ConnectInfo[eConnectGeneral].ConnectValidTime = 1;
	APPConnect.ConnectInfo[eConnectGeneral].ConstConnectValidTime = 1;//1s后关闭一般密码 与魏工商量无论客户机为什么都关闭一般密码
	
	APPData[Ch].APPFlag = APP_HAVE_DATA;
	APPData[Ch].APPBufLen.w = 4;
	APPData[Ch].BufDealLen.w = LinkData[Ch].pAPPLen.w;
}

//--------------------------------------------------
//功能描述:    获取电表运行状态字1
//         
//参数:      BYTE* Buf[in]
//         
//返回值:     无 
//         
//备注内容:    无
//--------------------------------------------------
void GetMeterStatus1( BYTE* Buf)
{
	/**********************定义************************/
	//bit15	时钟故障
	//bit14	透支状态
	//bit13	存储器故障或损坏
	//bit12	内部程序错误
	//bit11	保留
	//bit10	保留
	//bit9	esam错误
	//bit8	控制回路错误
	//bit7	保留
	//bit6	计量芯片故障
	//bit5	无功功率方向( 0 正向，1反向)
	//bit4	有功功率方向( 0 正向，1反向)
	//bit3	停电抄表电池( 0 正常, 1欠压)
	//bit2	时钟电池( 0 正常, 1欠压)
	//bit1	需量计算方式( 0 滑差, 1 区间)
	//bit0	保留
	/**********************高字节************************/
	if( ( api_GetRunHardFlag(eRUN_HARD_ERR_RTC_FLAG) == TRUE )					//BIT7	时钟故障	
	|| ( api_GetRunHardFlag(eRUN_HARD_BROADCAST_ERR_FLAG) == TRUE))
	{
		Buf[0] |= BIT7;
	}

	if( api_GetSysStatus( eSYS_STATUS_PRE_USE_MONEY ) == TRUE )	//BIT6	透支状态	
	{
		Buf[0] |= BIT6;
	}

    if((api_CheckError(ERR_WRITE_EEPROM1) == TRUE)//e2     
    ||(api_CheckError(ERR_WRITE_EEPROM2) == TRUE)       //BIT5	存储器故障或损坏	
    ||(api_CheckError(ERR_WRITE_EEPROM3) == TRUE))      //BIT4	内部程序错误	 											     	
    {
       Buf[0] |= BIT5;
    }
                                                            //BIT4	内部程序错误
															//BIT3	保留			
															//BIT2	保留			
	if( api_GetRunHardFlag( eRUN_HARD_ESAM_ERR ) == TRUE )		//BIT1	esam错误		
	{
		Buf[0] |= BIT1;
	}	

	if( api_GetSysStatus( eSYS_STATUS_RELAY_ERR ) == TRUE )		//BIT0	控制回路错误		
	{
		Buf[0] |= BIT0;
	}	
	/**********************低字节************************/
															//BIT7	保留			
	if( api_CheckError( ERR_CHECK_5460_1 ) == TRUE )		//BIT6	计量芯片故障		
	{
		Buf[1] |= BIT6;
	}
	
	if( api_GetSysStatus( eSYS_STATUS_OPPOSITE_Q ) == TRUE )		//BIT5	无功功率方向( 0 正向，1反向)		
	{
		Buf[1] |= BIT5;
	}	

	if( api_GetSysStatus( eSYS_STATUS_OPPOSITE_P ) == TRUE )		//BIT4	有功功率方向( 0 正向，1反向)		
	{
		Buf[1] |= BIT4;
	}

	//if( api_GetRunHardFlag( eRUN_HARD_READ_BAT_LOW ) == TRUE )	//BIT3	停电抄表电池( 0 正常, 1欠压)		
	//{
	//	Buf[1] |= BIT3;
	//}

	if( api_GetRunHardFlag( eRUN_HARD_CLOCK_BAT_LOW ) == TRUE )	//BIT2	时钟电池( 0 正常, 1欠压)			
	{
		Buf[1] |= BIT2;
	}

															//BIT1	需量计算方式( 0 滑差, 1 区间)		
															//BIT0	保留		
}

//--------------------------------------------------
//功能描述:    获取电表运行状态字2
//         
//参数:      BYTE* Buf[in]
//         
//返回值:     无 
//         
//备注内容:    无
//--------------------------------------------------
void GetMeterStatus2( BYTE *Buf)
{
	/**********************定义************************/
	//bit15	保留
	//bit14	保留
	//bit13	保留
	//bit12	保留
	//bit11	保留
	//bit10	保留
	//bit9	保留
	//bit8	保留
	//bit7	保留
	//bit6	C相无功功率方向( 0 正向，1反向)
	//bit5	B相无功功率方向( 0 正向，1反向)
	//bit4	A相无功功率方向( 0 正向，1反向)
	//bit3	保留
	//bit2	C相有功功率方向( 0 正向，1反向)
	//bit1	B相有功功率方向( 0 正向，1反向)
	//bit0	A相有功功率方向( 0 正向，1反向)
	/**********************低字节************************/
	
	if( api_GetSysStatus( eSYS_STATUS_OPPOSITE_Q_C ) == TRUE )		//BIT6	C相无功功率方向( 0 正向，1反向)		
	{
		Buf[1] |= BIT6;
	}	

	if( api_GetSysStatus( eSYS_STATUS_OPPOSITE_Q_B ) == TRUE )		//BIT5	B相无功功率方向( 0 正向，1反向)		
	{
		Buf[1] |= BIT5;
	}

	if( api_GetSysStatus( eSYS_STATUS_OPPOSITE_Q_A ) == TRUE )		//BIT4	A相无功功率方向( 0 正向，1反向)		
	{
		Buf[1] |= BIT4;
	}
															//bit3	保留
	if( api_GetSysStatus( eSYS_STATUS_OPPOSITE_P_C ) == TRUE )		//BIT2	C相无功功率方向( 0 正向，1反向)		
	{
		Buf[1] |= BIT2;
	}	

	if( api_GetSysStatus( eSYS_STATUS_OPPOSITE_P_B ) == TRUE )		//BIT1	BIT1B相无功功率方向( 0 正向，1反向)		
	{
		Buf[1] |= BIT1;
	}

	if( api_GetSysStatus( eSYS_STATUS_OPPOSITE_P_A ) == TRUE )		//BIT0	A相无功功率方向( 0 正向，1反向)		
	{
		Buf[1] |= BIT0;
	}
}

//--------------------------------------------------
//功能描述:  获取电表运行状态字3
//         
//参数:     BYTE* Buf[in]
//         
//返回值:    无  
//         
//备注内容:  无
//--------------------------------------------------
void GetMeterStatus3( ePROTOCO_TYPE ProtocolType, BYTE *Buf)
{	
    BYTE DataBuf[4];
    
    memset( Buf, 0x00, 2 );
	//高字节
	//bit1//bit0	电能表类型 00 非预付费表 01电量型预付费表 10电费型预付费表
	if( PREPAY_MODE == PREPAY_LOCAL )
	{	
		Buf[0] |= BIT1; 
	}
	//bit7		远程开户(0开户,1未开户)
	//bit6		本地开户(0开户,1未开户)
    #if( PREPAY_MODE == PREPAY_LOCAL )
    Buf[0] |= BIT6+BIT7;
    if( api_ReadPrePayPara( eLocalAccountFlag, DataBuf ) != 0 )
    {
        if( DataBuf[0] == eOpenAccount )//bit6		本地开户(0开户,1未开户)			//bit14
		{
			Buf[0] &= ~BIT6;
		}
    }
	if( api_ReadPrePayPara( eRemoteAccountFlag, DataBuf ) != 0 )
    {
        if( DataBuf[0] == eOpenAccount )//bit6		远程开户(0开户,1未开户)			//bit14
		{
			Buf[0] &= ~BIT7;
		}
    }
    #endif//#if( PREPAY_MODE == PREPAY_LOCAL )
    
	if( ProtocolType == ePROTOCO_645 ) //645协议读取
	{
    	if( api_GetSysStatus( eSYS_STATUS_ID_645AUTH ) == TRUE)//bit5		安全认证状态(0失效,1有效)
    	{
    		Buf[0] |= BIT5;
    	}
	}
	else//698协议读取
	{
    	if( api_GetSysStatus( eSYS_STATUS_ID_698MASTER_AUTH ) == TRUE )//bit5 主站安全认证状态(0失效,1有效)
    	{
    		Buf[0] |= BIT5;
    	}

    	if( api_GetSysStatus( eSYS_STATUS_ID_698TERMINAL_AUTH ) == TRUE)//bit3 终端安全认证状态(0失效,1有效)
    	{
    		Buf[0] |= BIT3;
    	}
	}

	if( api_GetSysStatus( eSYS_TERMINAL_ALLOW_PRG ) == TRUE )//bit2 终端身份认证状态
	{
		Buf[0] |= BIT2;
	}

	if( RelayTypeConst != RELAY_NO )
	{
		if( api_GetRelayStatus( 2 ) == TRUE )    //bit4		保电状态(0非保电,1保电)
		{
			Buf[0] |= BIT4;
		}
												//bit3		保留
												//bit2		保留
		
		#if( PREPAY_MODE == PREPAY_LOCAL )
		if( api_GetRelayStatus(7) != 0 )		//bit6		继电器命令状态(0通，1断)
		{
			Buf[1] |= BIT6;
		}
		#else
		if( api_GetRelayStatus(0) != 0 )		//bit6		继电器命令状态(0通，1断)
		{
			Buf[1] |= BIT6;
		}
		#endif

												//bit5		保留
		if( api_GetRelayStatus(1) != 0 )		//bit4		继电器实际状态(0通，1断)
		{
			Buf[1] |= BIT4;
		}
	}
	
	//低字节
	if( api_GetRelayStatus(3) == TRUE )			//bit7		预跳闸报警状态(0无，1有)
	{
		Buf[1] |= BIT7;
	}

	if( api_GetSysStatus( eSYS_IR_ALLOW_PRG ) == TRUE )			//bit3		编程允许状态(0失效,1有效)	
	{
		Buf[1] |= BIT3;
	}
	
    if( SelSecPowerConst==NO )
    {
        if( api_GetSysStatus( eSYS_STATUS_POWER_ON ) != TRUE )			//bit2//bit1	供电方式(00主电源,02辅助电源，01电池供电)	
    	{
    		Buf[1] |= BIT1;
    	}
    }
    else
    {
        if( api_GetSysStatus( eSYS_STATUS_POWER_ON ) != TRUE )			//bit2//bit1	供电方式(00主电源,01辅助电源，10电池供电)	
    	{
    		Buf[1] |= BIT2;
    	}
    	else
    	{
    	    #if( SEL_EVENT_LOST_SECPOWER == YES )//36
        	if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_LOST_SECPOWER ) == 0 )	//辅助电源失电
    		{
    			Buf[1] |= BIT1;
    		}
    		#endif
    	}
    }
}

//--------------------------------------------------
//功能描述:  获取电表运行状态字4,5,6
//         
//参数:     BYTE Phase[in] 0:A相, 1:B相, 2:C相
//         
//        BYTE* Buf[in]
//         
//返回值:    无  
//         
//备注内容:  无
//--------------------------------------------------
void GetMeterStatus4_5_6( BYTE Phase,BYTE *Buf)
{
#if( MAX_PHASE == 1 )
    if( Phase > 0 )
    {
        return;
    }
#endif
    /**********************定义************************/
	//bit15		保留
	//bit14		保留
	//bit13		保留
	//bit12		保留
	//bit11		保留
	//bit10		保留
	//bit9		保留
	//bit8		断流
	//bit7		断相
	//bit6		功率反向
	//bit5		过载
	//bit4		过流
	//bit3		失流
	//bit2		过压
	//bit1		欠压
	//bit0		失压
#if( SEL_EVENT_BREAK_I == YES )
	if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_BREAK_I_A+Phase) == 1 )		//bit1		A断流
	{
		Buf[0] |= BIT0;
	}
#endif
	/**********************低字节************************/
#if( SEL_EVENT_BREAK == YES )
	if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_BREAK_A+Phase) == 1 )		//bit7		A断相
	{
		Buf[1] |= BIT7;
	}
#endif

#if( SEL_EVENT_BACKPROP == YES )
	if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_POW_BACK_A+Phase) == 1 )	//bit6		A功率反向
	{
		Buf[1] |= BIT6;
	}
#endif			

#if( SEL_EVENT_OVERLOAD == YES )
	if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_OVERLOAD_A+Phase) == 1 )	//bit5		A过载
	{
		Buf[1] |= BIT5;
	}
#endif	

#if( SEL_EVENT_OVER_I == YES )
	if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_OVER_I_A+Phase) == 1 )		//bit4		A过流
	{
		Buf[1] |= BIT4;
	}
#endif	

#if( SEL_EVENT_LOST_I == YES )
	if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_LOSTI_A+Phase) == 1 )		//bit3		A失流
	{
		Buf[1] |= BIT3;
	}
#endif

#if( SEL_EVENT_OVER_V == YES )
	if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_OVER_V_A+Phase) == 1 )		//bit2		A过压
	{
		Buf[1] |= BIT2;
	}
#endif

#if( SEL_EVENT_WEAK_V == YES )
	if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_WEAK_V_A+Phase) == 1 )		//bit1		A欠压
	{
		Buf[1] |= BIT1;
	}
#endif

#if( SEL_EVENT_LOST_V == YES )
	if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_LOSTV_A+Phase) == 1 )		//bit0		A失压
	{
		Buf[1] |= BIT0;
	}
#endif
}

//--------------------------------------------------
//功能描述:    获取电表运行状态字7
//         
//参数:      BYTE* Buf[in]
//         
//返回值:     无 
//         
//备注内容:    无
//--------------------------------------------------
void GetMeterStatus7( BYTE *Buf )
{
    BYTE i;
	/**********************定义************************/
	//bit15		保留
	//bit14		保留
	//bit13		保留
	//bit12		保留
	//bit11		保留
	//bit10		开端钮盖
	//bit9		开表盖
	//bit8		电流严重不平衡
	//bit7		总功率因数超下限
	//bit6		需量超限
	//bit5		掉电
	//bit4		辅助电源失电
	//bit3		电流不平衡
	//bit2		电压不平衡
	//bit1		电流逆向序
	//bit0		电压逆相序
	/**********************高字节************************/
#if( SEL_EVENT_BUTTONCOVER == YES )
	if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_BUTTON_COVER ) == 1 )	//bit2		开端钮盖
	{
		Buf[0] |= BIT2;
	}
#endif

#if( SEL_EVENT_METERCOVER == YES )
	if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_METER_COVER ) == 1 )	//bit1		开表盖
	{
		Buf[0] |= BIT1;
	}
#endif

#if( SEL_EVENT_I_S_UNBALANCE == YES )
	if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_S_I_UNBALANCE ) == 1 )	//bit0		电流严重不平衡
	{
		Buf[0] |= BIT0;
	}
#endif

#if( SEL_EVENT_COS_OVER == YES )
	if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_COS_OVER_ALL ) == 1 )	//bit7		总功率因数超下限
	{
		Buf[1] |= BIT7;
	}
#endif

#if( SEL_DEMAND_OVER == YES )
    for( i=0; i < 6; i++ )//只要有一项需量超限就认为需量超限
    {
        if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_PA_DEMAND_OVER+i ) == 1 )    //bit6      需量超限
        {
            Buf[1] |= BIT6;
            break;
        }
    }
#endif

#if( SEL_EVENT_LOST_POWER == YES )
	if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_LOST_POWER ) == 1 )	//bit5		掉电
	{
		Buf[1] |= BIT5;
	}
#endif

#if( SEL_EVENT_LOST_SECPOWER == YES )
	if( SelSecPowerConst == YES )
	{
		if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_LOST_SECPOWER ) == 1 )	//bit4		辅助电源失电
		{
			Buf[1] |= BIT4;
		}
	}
#endif

#if( SEL_EVENT_I_UNBALANCE == YES )
	if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_I_UNBALANCE ) == 1 )		//bit3		电流不平衡
	{
		Buf[1] |= BIT3;
	}
#endif

#if( SEL_EVENT_V_UNBALANCE == YES )
	if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_V_UNBALANCE ) == 1 )	//bit2		电压不平衡
	{
		Buf[1] |= BIT2;
	}
#endif

#if( SEL_EVENT_I_REVERSAL == YES )
	if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_I_REVERSAL ) == 1 )		//bit1		电流逆向序
	{
		Buf[1] |= BIT1;
	}
#endif

#if( SEL_EVENT_V_REVERSAL == YES )
	if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_V_REVERSAL ) == 1 )		//bit0		电压逆相序
	{
		Buf[1] |= BIT0;
	}
#endif
}
//--------------------------------------------------
//功能描述:    获取电表运行状态字
//         
//参数:      BYTE Type[in]   Type 0 全部电表运行状态字  
//         		           Type 1 ~ 7 代表电表运行状态字 1 ~ 7
//         BYTE BufLen[in]
//         
//         BYTE Buf[in]    每两个字节代表1组 Buf[0] 代表bit15 ~ bit8； Buf[1] 代表bit8 ~ bit0
//         
//返回值:     WORD  0x8000    代表出现错误 0x0000代表buf长度不够 其他代表正常返回长度
//         
//备注内容:    无
//--------------------------------------------------
WORD api_GetMeterStatus( ePROTOCO_TYPE ProtocolType, BYTE Type, WORD BufLen, BYTE *Buf )
{
	WORD Len;

	if( Type > 7 )
	{
		return 0x8000;
	}
	
	if( ( Type ==0 ) && ( BufLen < 14) )
	{		
		return 0;
	}
	else if( BufLen < 2)
	{
		return 0;
	}
	
	if( BufLen < 14 )
	{
		memset( Buf,0x00,BufLen);	//对Buf进行清零操作
	}
	else
	{
		memset( Buf,0x00,14);		//对Buf进行清零操作
	}	
	
	switch( Type )
	{
		case 0://电表运行状态字
			GetMeterStatus1( Buf );
			GetMeterStatus2( Buf+2 );
			GetMeterStatus3( ProtocolType, Buf + 4 );
			GetMeterStatus4_5_6(0,Buf +6);
			GetMeterStatus4_5_6(1,Buf +8);
			GetMeterStatus4_5_6(2,Buf +10);
			GetMeterStatus7( Buf+12 );
			Len = 14;
			break;
		case 1://电表运行状态字1
			GetMeterStatus1( Buf );	
			Len = 2;
			break;			
		case 2://电表运行状态字2
			GetMeterStatus2( Buf );
			Len = 2;
			break;			
		case 3://电表运行状态字3
			GetMeterStatus3( ProtocolType, Buf );
			Len = 2;
			break;	
		case 4://电表运行状态字4
		case 5://电表运行状态字5
		case 6://电表运行状态字6
			GetMeterStatus4_5_6( (Type -4) ,Buf );
			Len = 2;
			break;	
		case 7://电表运行状态字7
			GetMeterStatus7(Buf);
			Len = 2;
			break;
	}

    return Len;
}
//--------------------------------------------------
//功能描述:    读取Normal结果处理
//         
//参数:      BYTE Ch[in]   通道选择
//         
//         WORD Len[in]  数据长度
//         
//返回值:     eAPPBufResultChoice 返回buf添加结果
//         
//备注内容:    无
//--------------------------------------------------
static eAPPBufResultChoice DealGetRequestNormalResult( BYTE Ch,WORD Len)
{
	eAPPBufResultChoice eResultChoice;
	
	if( (!(Len & 0x8000)) && ( Len!= 0) )//返回长度bit7不为1
	{
		eResultChoice = DLT698AddOneBuf(Ch,0, 1);//响应数据   CHOICE  {     错误信息     [0] DAR，   M条记录     [1] SEQUENCE OF A-RecordRow}
		if( eResultChoice != eADD_OK )
		{
			return eResultChoice;
		}
		eResultChoice = DLT698AddBufLen( Ch, 0x55, Len);
	}
	else//提示上层进行分帧
	{
		if( Len == 0)//buf长度不够
		{
			eResultChoice = DLT698AddBufDAR( Ch, DAR_OverRegion );
		}
		else
		{
			eResultChoice = DLT698AddBufDAR( Ch, DAR_Undefined );
		}	
	}
	
	return eResultChoice;
}

//--------------------------------------------------
//功能描述:    读取Normal数据
//         
//参数:      BYTE Ch[in]     通道选择
//         
//         BYTE *pOAD[in]  OAD指针
//         
//返回值:     eAPPBufResultChoice 返回buf添加结果
//         
//备注内容:    无
//--------------------------------------------------
static eAPPBufResultChoice GetRequestNormal( BYTE Ch,BYTE *pOAD)
{
	WORD Len;
	eAPPBufResultChoice eResultChoice;
	TTwoByteUnion OI;

	lib_ExchangeData(OI.b,pOAD,2);
	if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return eAPP_ERR_RESPONSE;
    }
    
	eResultChoice = DLT698AddBuf(Ch,0, pOAD, 4);//添加OAD数据 不是完整数据
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	
    APPData[Ch].BufDealLen.w +=4;//已处理4字节oad

    if( APPData[Ch].TimeTagFlag == eTime_Invalid )
    {
		eResultChoice = DLT698AddBufDAR( Ch,DAR_TimeStamp );
		if( eResultChoice != eADD_OK )
		{
			return eResultChoice;
		}
    }
	else if(JudgeTaskAuthority( Ch, eREAD_MODE, pOAD)== FALSE )//判断安全模式参数
	{
		eResultChoice = DLT698AddBufDAR( Ch,DAR_PassWord );
		if( eResultChoice != eADD_OK )
		{
			return eResultChoice;
		}
	}
	else
	{
		if( (APPData[Ch].APPMaxBufLen.w <= (APPData[Ch].APPCurrentBufLen.w+1)) //进行极限值判断
	    ||(APPData[Ch].APPMaxBufLen.w > MAX_APDU_SIZE))
		{
            Len = 0;
		}
		else
		{
			Len = GetProOadData(eGetNormalData, Ch, 1, 0XFF, pOAD, (BYTE *)NULL, (APPData[Ch].APPMaxBufLen.w - APPData[Ch].APPCurrentBufLen.w - 1),
			                    &APPData[Ch].pAPPBuf[APPData[Ch].APPCurrentBufLen.w + 1]);   // 添加数据buf
		}

		eResultChoice = DealGetRequestNormalResult( Ch, Len);
	}
	
	return eResultChoice;
}

//--------------------------------------------------
//功能描述:    读取NormalList数据
//         
//参数:      BYTE Ch[in]     通道选择   
//         
//返回值:     eAPPBufResultChoice 返回buf添加结果
//         
//备注内容:    无
//--------------------------------------------------
eAPPBufResultChoice GetRequestNormalList( BYTE Ch)
{	
	WORD i,NoAddress,LenOffest;
    TTwoByteUnion Num,No;
	eAPPBufResultChoice eResultChoice;
	
	if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return eAPP_ERR_RESPONSE;
    }

    LenOffest = Deal_698DataLenth( (BYTE*)&LinkData[Ch].pAPP[APPData[Ch].BufDealLen.w], Num.b, ePROTOCOL_TYPE ); 
	APPData[Ch].BufDealLen.w +=LenOffest;
	
	No.w = 0;
	
	eResultChoice = DLT698AddOneBuf(Ch,0, Num.b[0] );//添加个数 产生后续帧不正确
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	
    NoAddress = APPData[Ch].APPCurrentBufLen.w;
    
	for(i = 0;i < Num.w;i++)//如果读取的OAD太多，要用后续帧 for debug
	{
		eResultChoice = GetRequestNormal( Ch,&LinkData[Ch].pAPP[APPData[Ch].BufDealLen.w]);

        if( eResultChoice != eADD_OK) 
		{
			break;
		}
		
		No.w++;//sequence of  a_result_normal 加1	
		
		if( APPData[Ch].BufDealLen.w >= LinkData[Ch].pAPPLen.w )//已处理数据大于下发的数据进行返回，避免错误报文的干扰
		{
			break;
		}
	}	
	
    JudgeAppBufLen(Ch);

    Deal_698SequenceOfNumData( Ch, NoAddress, No.b);
    
	return eResultChoice;
}

//--------------------------------------------------
//功能描述:     规约读取记录函数
//         
//参数:       BYTE Tag[in] 						 是否加TAG标志;	0x02代表需要显示需要读取； 0x01代表需要加TAG； 	   	  0x00 代表不加TAG； 
//							       
//          TDLT698RecordPara *DLT698RecordPara[in] 读取记录或事件的参数结构体
//         
//          WORD BufLen[in]				     输入的buf长度 用来判断buf长度是否够用
//         
//          BYTE *pBuf[in]						 输入buf指针
//         
//返回值:      WORD								 bit15位置1 代表记录或冻结无此类型 置0代表数据能正确返回； 
//											     bit0~bit14 代表返回的冻结或事件的字节数
//         
//备注内容:  无
//--------------------------------------------------
WORD ReadProRecordData( BYTE Tag, TDLT698RecordPara *pDLT698RecordPara, WORD BufLen,BYTE *pBuf )
{
	WORD Len;
	Len = 0x8000;
	
	if( ( (pDLT698RecordPara->OI >= 0x3000) && (pDLT698RecordPara->OI <= 0x310f) )
    #if (SEL_EVENT_FIND_UNKNOWN_METER == YES )
    || (pDLT698RecordPara->OI == 0x3111) //发现未知电能表事件
    #endif
    #if (SEL_TOPOLOGY == YES )
    || (pDLT698RecordPara->OI == 0x3E03) //特征电流信号识别结果事件
    #endif
	)//调用事件函数
	{
	    if( ((pDLT698RecordPara->Phase == ePHASE_B) && (MeterTypesConst == METER_3P3W)) 
	    && (pDLT698RecordPara->OI != 0x300b) )//三相三线表除0x300b无功需量超限外不支持B相
	    {
            Len = 0x8000;
	    }
	    else
	    {
            Len = api_ReadEventRecord( Tag, pDLT698RecordPara, BufLen, pBuf);
	    }
	}
	else if((pDLT698RecordPara->OI >= 0x5000) && (pDLT698RecordPara->OI <= 0x5011))//调用冻结函数
	{
		Len = api_ReadFreezeRecord( Tag, pDLT698RecordPara, BufLen, pBuf);
	}
	else if ((pDLT698RecordPara->OI >= 0xBD00) && (pDLT698RecordPara->OI <= 0xBD09))
	{
		Len = api_ReadEventRecord( Tag, pDLT698RecordPara, BufLen, pBuf);
	}

	return Len;//返回数据对象不存在
}

//--------------------------------------------------
//功能描述:     规约读取记录函数
//         
//参数:       BYTE Tag[in] 						 是否加TAG标志;	0x02代表需要显示需要读取； 0x01代表需要加TAG； 	   	  0x00 代表不加TAG； 
//							       
//          TDLT698RecordPara *DLT698RecordPara[in] 读取记录或事件的参数结构体
//         
//          WORD BufLen[in]				     输入的buf长度 用来判断buf长度是否够用
//         
//          BYTE *pBuf[in]						 输入buf指针
//         
//返回值:      WORD								 bit15位置1 代表记录或冻结无此类型 置0代表数据能正确返回； 
//											     bit0~bit14 代表返回的冻结或事件的字节数
//         
//备注内容:  无
//--------------------------------------------------
WORD api_ReadProRecordData( BYTE Tag, TDLT698RecordPara *pDLT698RecordPara, WORD BufLen,BYTE *pBuf )
{	
    const DWORD (* pEvent)[2];
	TFourByteUnion OAD;
	BYTE i;

	if( Tag == 2 )//显示读取 
	{
		if( (((pDLT698RecordPara->OI) >= 0x3000) && ((pDLT698RecordPara->OI) < 0x3030) && ((pDLT698RecordPara->OI) != 0x300C))
		  ||((pDLT698RecordPara->OI) == 0x303B)||((pDLT698RecordPara->OI) == 0x303C)  ||((pDLT698RecordPara->OI) == 0x3040)
		  ||((pDLT698RecordPara->OI) == 0x30E0)||((pDLT698RecordPara->OI) == 0x30E1)||((pDLT698RecordPara->OI) == 0x30E2)
		  ||((pDLT698RecordPara->OI) == 0x3053)||((pDLT698RecordPara->OI) == 0x310f)
		  ||((pDLT698RecordPara->OI) == 0x3104) 
          #if (SEL_EVENT_FIND_UNKNOWN_METER == YES )
          || (pDLT698RecordPara->OI == 0x3111) //发现未知电能表事件
          #endif
          #if (SEL_TOPOLOGY == YES )
          || (pDLT698RecordPara->OI == 0x3E03) //特征电流信号识别结果事件
          #endif
		  ) //事件RCSD 		
		  {
			if ((pDLT698RecordPara->OI) == 0x303B)
			{
				pEvent = pEventUnit[0x30];//获取对应的事件记录表
			}
			else if((pDLT698RecordPara->OI) == 0x303C)
			{
				pEvent = pEventUnit[0x31];//获取对应的事件记录表
			}
			else if ((pDLT698RecordPara->OI) == 0x3040)
			{
				pEvent = pEventUnit[0x32];//获取对应的事件记录表
			}
			else if ((pDLT698RecordPara->OI) ==0x30E0)
			{
				pEvent=ChargingWARNINGUnit;
			}
			else if ((pDLT698RecordPara->OI) ==0x30E1)
			{
				pEvent=ChargingERRUnit;
			}
			else if ((pDLT698RecordPara->OI) ==0x30E2)
			{
				pEvent=ChargingCOM_EXCTIONUnit;
			}
			else if(((pDLT698RecordPara->OI) == 0x3053) || ((pDLT698RecordPara->OI) == 0x310f))
			{
				pEvent = StandardEventUnit;
			}
			else if ((pDLT698RecordPara->OI) == 0x3104)
			{
				pEvent = pEventUnit[0x33];//获取对应的事件记录表
			}
            #if (SEL_EVENT_FIND_UNKNOWN_METER == YES )
            else if( (pDLT698RecordPara->OI == 0x3111) )//发现未知电能表事件
            {
                pEvent = FindUnknownMeterUnit;
            }
            #endif
            #if (SEL_TOPOLOGY == YES )
            else if( (pDLT698RecordPara->OI == 0x3E03) )//特征电流信号识别结果事件
            {
                pEvent = TopoUnit;
            }
            #endif
			else
			{
				pEvent = pEventUnit[(pDLT698RecordPara->OI)-0x3000];//获取对应的事件记录表
			}

			if( (pDLT698RecordPara->OADNum) != 1 )
			{
				return 0x8000;
			}

			memcpy( OAD.b,pDLT698RecordPara->pOAD,4);
			for( i = 0; i < pEvent[0][0]; i++ )
			{
				if( OAD.d == pEvent[i+1][0] )//查表
				{
					memcpy(pDLT698RecordPara->pOAD,&pEvent[i+1][1],4);//组帧RCSD
					break;
				}
			
			}
		}
		Tag = 0;//TAG置成不需要加TAG的
	}
	
    pDLT698RecordPara->Ch = 0x55;//外部调用时 统一置为0x55 避免影响跟随上报
    
    return ReadProRecordData( Tag, pDLT698RecordPara, BufLen, pBuf);

}

//--------------------------------------------------
//功能描述:    下行RCSD为零时数据处理
//         
//参数:      BYTE Ch[in]   通道选择
//         
//         DWORD OI[in]  记录OI
//
//         BYTE *DAR[out] 错误DAR
//         
//返回值:     eAPPBufResultChoice  返回添加BUF结果
//         
//备注内容:    无
//--------------------------------------------------
eAPPBufResultChoice DealRCSDIsZeroData( BYTE Ch, WORD OI, BYTE *DAR)
{
	WORD Len;
	BYTE Num;
	BYTE *pRCSDNum;
	WORD i;
	eAPPBufResultChoice eResultChoice;
	const DWORD (*pEvent)[2];
	
	pRCSDNum = &APPData[Ch].pAPPBuf[APPData[Ch].APPCurrentBufLen.w];//取得rcsd个数的指针
	eResultChoice = DLT698AddOneBuf( Ch, 0, 1);				//添加rcsd的 个数
	
	if( ( (OI >= 0x3000) && ( OI < 0x3030) && ( OI != 0x300C))	
	|| (OI == 0x303B) || (OI == 0x303C) || ( OI == 0x3040)|| ( OI == 0x30E0)
	|| ( OI == 0x30E1)|| ( OI == 0x30E2)||((OI >= 0xBD00)&&(OI<=0xBD09) )
	|| ( OI == 0x3053)|| ( OI == 0x310f)|| ( OI == 0x3104)
    #if (SEL_EVENT_FIND_UNKNOWN_METER == YES )
    || (OI == 0x3111) //发现未知电能表事件
    #endif
    #if (SEL_TOPOLOGY == YES )
    || (OI == 0x3E03) //特征电流信号识别结果事件
    #endif
	)//事件RCSD 
	{
		if(OI == 0x303B)
		{
			pEvent = pEventUnit[0x30];		//获取对应的事件记录表
		}
		else if(OI == 0x303C)
		{
			pEvent = pEventUnit[0x31];		//获取对应的事件记录表
		}
		else if( OI == 0x3040)
		{
			pEvent = pEventUnit[0x32];		//获取对应的事件记录表
		}
		else if (OI ==0x30E0)
		{
			pEvent=ChargingWARNINGUnit;
		}
		else if (OI ==0x30E1)
		{
			pEvent=ChargingERRUnit;
		}
		else if (OI ==0x30E2)
		{
			pEvent=ChargingCOM_EXCTIONUnit;
		}
		else if( OI == 0x3104)
		{
			pEvent = pEventUnit[0x33];		//获取对应的事件记录表
		}
		else if ((OI >= 0xBD00)&&(OI<=0xBD09))
		{
			pEvent = ModuleEventUnit;
		}
		else if((OI == 0x3053) || (OI == 0x310f))
		{
			pEvent = StandardEventUnit;
		}
        #if (SEL_EVENT_FIND_UNKNOWN_METER == YES )
        else if(OI == 0x3111) //发现未知电能表事件
        {
            pEvent = FindUnknownMeterUnit;
        }
        #endif
        #if (SEL_TOPOLOGY == YES )
        else if( OI == 0x3E03 )//特征电流信号识别结果事件
        {
            pEvent = TopoUnit;
        }
        #endif
		else
		{
			pEvent = pEventUnit[OI-0x3000];		//获取对应的事件记录表
		}
		
		
		for( i=0; i< pEvent[0][0]; i++ )		//添加固定列
		{
			eResultChoice = DLT698AddOneBuf( Ch, 0, 0);//添加rcsd的 choice
			if( eResultChoice != eADD_OK )
			{
				return eResultChoice;
			}

			eResultChoice = DLT698AddBuf( Ch, 0, (BYTE*)& pEvent[i+1][1], 4);
			if( eResultChoice != eADD_OK )
			{
				return eResultChoice;
			}
		}
	
		Len = api_ReadEventAttribute(OI,0,MAX_APDU_SIZE, ProtocolBuf);//返回结果无法处理 因此不判断

		if( (Len & 0x8000) ||(Len < 4))//返回错误或者长度小于1个oad长度
		{
			Num =0;
		}
		else
		{
			Num = Len/4;
		}
		
		if( Num > MAX_EVENT_OAD_NUM )//进行极限值的判断
		{
			Num = MAX_EVENT_OAD_NUM;
		}
		
		for( i=0; i < Num; i++ )//添加关联对象属性
		{
			eResultChoice = DLT698AddOneBuf( Ch, 0, 0);//添加rcsd的 choice
			if( eResultChoice != eADD_OK )
			{
				return eResultChoice;
			}
			
			eResultChoice = DLT698AddBuf( Ch, 0, &ProtocolBuf[4*i], 4);
			if( eResultChoice != eADD_OK )
			{
				return eResultChoice;
			}
		}

		pRCSDNum[0] = (Num+pEvent[0][0]);
	}
	else if(( OI >= 0x5000) && ( OI <= 0x5011))//冻结RCSD 
	{
		for( i=0; i< FixedFreezeOAD[0]; i++ )//添加固定列
		{
			eResultChoice = DLT698AddOneBuf( Ch, 0, 0);//添加rcsd的 choice
			if( eResultChoice != eADD_OK )
			{
				return eResultChoice;
			}
			
			eResultChoice = DLT698AddBuf( Ch, 0, (BYTE *)&FixedFreezeOAD[1+i], 4);
			if( eResultChoice != eADD_OK )
			{
				return eResultChoice;
			}
		}
		
		Len = api_ReadFreezeAttribute(OI,0,MAX_APDU_SIZE, ProtocolBuf);//返回结果无法处理 因此不判断

		if( (Len & 0x8000) ||(Len < 8))//返回错误或者长度小于1个oad长度
		{
			Num =0;
		}
		else
		{
			Num = Len/8;
		}
		
		if( Num > MAX_FREEZE_ATTRIBUTE )//进行极限值的判断
		{
			Num = MAX_FREEZE_ATTRIBUTE;
		}
		
		for( i=0; i < Num; i++ )//添加关联对象属性
		{
			eResultChoice = DLT698AddOneBuf( Ch, 0, 0);//添加rcsd的 choice
			if( eResultChoice != eADD_OK )
			{
				return eResultChoice;
			}
			
			eResultChoice = DLT698AddBuf( Ch, 0, &ProtocolBuf[2+8*i], 4);
			if( eResultChoice != eADD_OK )
			{
				return eResultChoice;
			}
		}

		pRCSDNum[0] = (Num+FixedFreezeOAD[0]);
	}
	else//如果是不支持的冻结或者事件 无法获取关联对象属性、填零代表全部
	{
        pRCSDNum[0] = 0;
        DAR[0] = DAR_WrongType;
	}

	return eResultChoice;
}

//--------------------------------------------------
//功能描述:    下行RCSD数据处理
//         
//参数:      BYTE Ch[in]	     通道选择
//         
//         WORD OI[in]	     OI 判断事件 冻结
//         
//         BYTE *pRCSD[in]	 RCSD指针		
//         
//         BYTE *OutBuf[in]  输出buf指针
//         
//         BYTE *DAR[out]	 错误标志
//         
//返回值:     eAPPBufResultChoice 添加数据结果
//         
//备注内容:    无
//--------------------------------------------------
eAPPBufResultChoice DealRCSDData( BYTE Ch, WORD OI,BYTE *pRCSD, BYTE *OutBuf,BYTE *DAR)
{
	BYTE i,n,LenOffest;
	TTwoByteUnion Len;
	eAPPBufResultChoice eResultChoice;
	const DWORD (* pEvent)[2];
	TFourByteUnion OAD;
	
	eResultChoice = eADD_OK;
	
	if( pRCSD[0] == 0 )
	{	
        eResultChoice = DealRCSDIsZeroData( Ch, OI, DAR);     
	}
	else
	{
	    LenOffest =  Deal_698DataLenth( pRCSD, Len.b, ePROTOCOL_TYPE ); 
	    if( Len.w > (MAX_EVENT_OAD_NUM+10) )//下发的oad个数不能超过Buf长度
	    {
            DAR[0] = DAR_WrongType;
            return eResultChoice;
	    }
	    
		eResultChoice = DLT698AddBuf(Ch,0, pRCSD, (Len.w*5+LenOffest));//添加RCSD数据 不是完整数据
		if( eResultChoice != eADD_OK )
		{
			return eResultChoice;
		}
		
		if( ( (OI >= 0x3000) && ( OI < 0x3030) && ( OI != 0x300C))
		 || (OI == 0x303B)  || (OI == 0x303C) || ( OI == 0x3040) || ( OI == 0x3104)
		 ||((OI >= 0xBD00) && ( OI < 0xBD09))
		 || ( OI == 0x30E0)||( OI == 0x30E1)|| ( OI == 0x30E2)
		 ||((OI == 0x3053) || ( OI == 0x310f)) 
         #if (SEL_EVENT_FIND_UNKNOWN_METER == YES )
         ||(OI == 0x3111)
         #endif
         #if (SEL_TOPOLOGY == YES )
         ||(OI == 0x3E03)
         #endif
         )//事件RCSD 
		{
			if(OI == 0x303B)
			{
				pEvent = pEventUnit[0x30];	//获取对应的事件记录表
			}
			else if(OI == 0x303C)
			{
				pEvent = pEventUnit[0x31];	//获取对应的事件记录表
			}
			else if( OI == 0x3040)
			{
				pEvent = pEventUnit[0x32];	//获取对应的事件记录表
			}
			else if( OI == 0x3104)
			{
				pEvent = pEventUnit[0x33];	//获取对应的事件记录表
			}
			else if (OI ==0x30E0)
			{
				pEvent=ChargingWARNINGUnit;
			}
			else if (OI ==0x30E1)
			{
				pEvent=ChargingERRUnit;
			}
			else if (OI ==0x30E2)
			{
				pEvent=ChargingCOM_EXCTIONUnit;
			}
			else if( (OI >= 0xBD00) && ( OI < 0xBD09) )//拓扑信息
			{
				pEvent = ModuleEventUnit;
			}
			else if((OI == 0x3053) || (OI == 0x310f))
			{
				pEvent = StandardEventUnit;
			}
            #if (SEL_EVENT_FIND_UNKNOWN_METER == YES )
            else if(OI == 0x3111)
            {
                pEvent = FindUnknownMeterUnit;
            }
            #endif
            #if (SEL_TOPOLOGY == YES )
            else if(OI == 0x3E03)
            {
                pEvent = TopoUnit;
            }
            #endif
			else
			{
				pEvent = pEventUnit[OI-0x3000];	//获取对应的事件记录表
			}

			for( i=0; i<Len.w;i++)			//获取调用函数的OAD
			{
			    if( pRCSD[LenOffest+2+5*i-2] != 0)
			    {
                    DAR[0] = DAR_WrongType;
                    break;
			    }
			    
				memcpy( OAD.b,&pRCSD[LenOffest+2+5*i-1],4);
				for( n = 0; n < pEvent[0][0]; n++ )
				{
					if( OAD.d == pEvent[n+1][0] )//查表
					{
						memcpy(&OutBuf[4*i],&pEvent[n+1][1],4);//组帧RCSD
						break;
					}					
				}
				
				if( n == pEvent[0][0] )
				{
					memcpy(&OutBuf[4*i],&pRCSD[LenOffest+2+5*i-1],4);//组帧RCSD
				}
			}
		
		}
		else if( (OI>=0x5000) && (OI<=0x5011) )
		{
			for( i=0; i<Len.w;i++)//获取调用函数的OAD
			{
			    if( pRCSD[LenOffest+2+5*i-2] != 0)
			    {
                    DAR[0] = DAR_WrongType;
                    break;
			    }
			    
				memcpy(&OutBuf[4*i],&pRCSD[LenOffest+2+5*i-1],4);//组帧RCSD
			}
		}
		else
		{
            DAR[0] = DAR_WrongType;
		}
	}

	return eResultChoice;

}
//--------------------------------------------------
//功能描述:    读取记录时象限与总、A、B、C的判断
//         
//参数:      WORD OI[in]				            OI
//         
//         BYTE ClassAttribute[in]		        属性
//         
//         TDLT698RecordPara *DLT698RecordPara[in] 记录结构体指针
//         
//返回值:     BOOL  TRUE 成功   FALSE 失败
//         
//备注内容:    无
//--------------------------------------------------
BOOL RecordJudgePhase( WORD OI, BYTE ClassAttribute, TDLT698RecordPara *DLT698RecordPara)
{
	if( ClassAttribute == 2 )//针对事件的象限判断
	{
		DLT698RecordPara->Phase = ePHASE_ALL;
	}
	else if( ClassAttribute >= 6 )
	{
		if( ClassAttribute == 6 )//属性9为总
		{
			if( (OI == 0x3007) || (OI == 0x300B) || (OI == 0x303B) )//OI不是潮流反向和无功超限 分项事件只有A,B,C相
			{
				DLT698RecordPara->Phase = ePHASE_ALL;
			}
			else
			{
				return FALSE;
			}

		}
		else
		{
			DLT698RecordPara->Phase = (ePHASE_TYPE)(ClassAttribute -6);		
			//象限不能超过4，且在单相时 不能抄读B，C相
			if( ((DLT698RecordPara->Phase) > 4) ||( (MAX_PHASE == 1) && ((DLT698RecordPara->Phase) > 1)) )
			{
				return FALSE;
			}
		}

	}
	else
	{
		DLT698RecordPara->Phase = ePHASE_ALL;
	}	
	
	return TRUE;
}

//--------------------------------------------------
//功能描述:    读取记录处理结果函数
//         
//参数:      BYTE Ch[in]  	通道
//         
//         WORD Len[in]	    记录返回的长度
//         
//返回值:     eAPPBufResultChoice   返回添加buf结果
//         
//备注内容:  无
//--------------------------------------------------
eAPPBufResultChoice DealGetRequestRecordResult( BYTE Ch,WORD Len)
{
	eAPPBufResultChoice eResultChoice ;
	
	if((!(Len & 0x8000)) && (Len !=0) )//如果是完整记录并且前面没有出错 //已经判断数组是否超限了 无需判断
	{
		eResultChoice = DLT698AddOneBuf(Ch,0, 1);//响应数据   CHOICE  {     错误信息     [0] DAR，   M条记录     [1] SEQUENCE OF A-RecordRow}
		if( eResultChoice != eADD_OK )
		{
			return eResultChoice;
		}

		eResultChoice = DLT698AddOneBuf(Ch,0, 1);// M条记录，M=1
		if( eResultChoice != eADD_OK )//
		{
			return eResultChoice;
		}
		
		eResultChoice = DLT698AddBufLen( Ch, 0x55, Len);
	}
	else
	{
		if( Len == 0 )//如果buf长度不够 进行应用层分帧
		{
			return eAPP_LINKDATA_OK;
		}
		else if( Len == 0x8000 )
		{
            //eResultChoice = DLT698AddBufDAR( Ch, DAR_NoObject );  //@@@@@@@  根据软件测试结果更改的
            eResultChoice = DLT698AddOneBuf(Ch,0, 1);//响应数据   CHOICE  {     错误信息     [0] DAR，   M条记录     [1] SEQUENCE OF A-RecordRow}
            if( eResultChoice != eADD_OK )
            {
                return eResultChoice;
            }
            
            eResultChoice = DLT698AddOneBuf(Ch,0X55, 0);// M条记录，M=0
            if( eResultChoice != eADD_OK )//
            {
                return eResultChoice;
            }
	    }
	    else
	    {
            eResultChoice = DLT698AddBufDAR( Ch,DAR_RefuseOp );
    		if( eResultChoice != eADD_OK )
    		{
    			return eResultChoice;
    		}
	    }
	}
	return eResultChoice;
}

//--------------------------------------------------
//功能描述:    RSD数据处理
//         
//参数:      BYTE *pRSD[in] RSD指针
//         
//         TDLT698RecordPara *DLT698RecordPara[in] 记录结构体指针
//         
//返回值:     BOOL TRUE 成功  FALSE 失败  
//         
//备注内容:    无
//--------------------------------------------------
BOOL DealRSDData( BYTE *pRSD, TDLT698RecordPara *DLT698RecordPara )
{
	TTwoByteUnion OI;

	lib_ExchangeData( OI.b, &pRSD[1], 2 );

	if( (OI.w == 0x2021) && (pRSD[5]==28) )
	{
		DLT698RecordPara->eTimeOrLastFlag = eFREEZE_TIME_FLAG;
	}
	else if( (OI.w == 0x201e) && (pRSD[5]==28) )
	{
		DLT698RecordPara->eTimeOrLastFlag = eEVENT_START_TIME_FLAG;
	}
	else if( (OI.w == 0x2020) && (pRSD[5]==28) )
	{
		DLT698RecordPara->eTimeOrLastFlag = eEVENT_END_TIME_FLAG;
	}
	else
	{
		return FALSE;
	}

	return TRUE;	
}
//--------------------------------------------------
//功能描述:    方法1读取记录
//         
//参数:      BYTE Ch[in]              通道选择
//         
//         BYTE *pRSD[in]           RSD的指针
//         
//         WORD OI[in]              数据OI
//         
//         BYTE ClassAttribute[in]  属性
//         
//返回值:     eAPPBufResultChoice      添加BUF操作结果
//         
//备注内容:    无
//--------------------------------------------------
eAPPBufResultChoice GetRequestRecordDataMethod1(BYTE Ch, BYTE *pRSD,WORD OI,BYTE ClassAttribute)
{
	BYTE *pRCSD,LenOffest,DAR;
	BYTE TimeBuf[7];
	BYTE Buf[(MAX_ATTRIBUTE+10)*4+20];
	WORD Len,Result,DataLen;
	DWORD AbsTime698;
	TTwoByteUnion TmpLen;
	eAPPBufResultChoice eResultChoice;
    TDLT698RecordPara DLT698RecordPara;
        
    DAR = DAR_Success;
	Len = 0;
	DataLen = 0;
	AbsTime698 = ILLEGAL_VALUE_8F;
	
	Result = DealRSDData( pRSD, ( TDLT698RecordPara * )&DLT698RecordPara);
	if( Result == FALSE )
	{	
        DAR = DAR_WrongType;	   
	}

    DataLen = GetProtocolDataLen( (BYTE*)&pRSD[5] );//获取DATA的数据长度
    if( DataLen == 0x8000 )//如果获取长度失败，暂时定为程序无应答
    {
        return eAPP_ERR_RESPONSE;    
    }
    
    pRCSD = pRSD+5+DataLen;
    //取下发时间
    if( pRSD[5] == DateTime_S_698 )//下发的时间格式必须是DateTime_S_698
    {
    	memcpy(TimeBuf,pRSD+6,7);
    	lib_InverseData(TimeBuf, 2);
    	
    	if( (OI >= 0x5000) && (OI <= 0x5011) )//冻结下发为分钟数
		{
        	AbsTime698 = api_CalcInTimeRelativeMin((TRealTimer *)TimeBuf);//计算起始时间
        	if( AbsTime698 == ILLEGAL_VALUE_8F )
        	{
                DAR = DAR_OverRegion;
        	}
            else if( TimeBuf[6] != 0 )//秒不为零时，下发的相对分钟数加1分钟，避免时间对应不上的问题
    		{
                AbsTime698 += 1;
    		}
		}
		else
		{
            AbsTime698 = api_CalcInTimeRelativeSec((TRealTimer *)TimeBuf);//计算起始时间
            if( AbsTime698 == ILLEGAL_VALUE_8F )
            {
                DAR = DAR_OverRegion;
            }
		}
    }
    else //解析数据长度
    {
    	DAR = DAR_WrongType;
    }

    //RCSD
    LenOffest =  Deal_698DataLenth( pRCSD, TmpLen.b, ePROTOCOL_TYPE );
    eResultChoice = DealRCSDData( Ch, OI, pRCSD,Buf, (BYTE *)&DAR);
    if( eResultChoice != eADD_OK )
    {
    	return eResultChoice;
    }
    
    if( APPData[Ch].TimeTagFlag == eTime_Invalid )//时间标签错误
    {
        DAR = DAR_TimeStamp;
    }
    else if( JudgeTaskAuthority( Ch, eREAD_MODE, pRSD-4) == FALSE )//判断安全模式参数 优先级别高
    {			
        DAR = DAR_PassWord;
    }
    else if( JudgeRecordOAD(OI,ClassAttribute) == FALSE )//判断记录OAD
    {			
        DAR = DAR_RefuseOp;

    }
    else
    {
        DLT698RecordPara.OI = OI;                   //OI数据
        DLT698RecordPara.Ch = Ch;
        DLT698RecordPara.pOAD = Buf;                //pOAD指针
        DLT698RecordPara.OADNum = TmpLen.w;     //OAD个数
        DLT698RecordPara.TimeOrLast = AbsTime698;   //绝对时间数
        Result = RecordJudgePhase( OI, ClassAttribute, &DLT698RecordPara);
        if( Result == FALSE )
        {
            DAR = DAR_RefuseOp;
        }   
    }
  
	if( DAR == DAR_Success )//无错误 进行读取记录
	{
		//调用函数 读取记录 返回数据 
		if( (APPData[Ch].APPMaxBufLen.w <= (APPData[Ch].APPCurrentBufLen.w+2)) //进行极限值判断
	    ||(APPData[Ch].APPMaxBufLen.w > MAX_APDU_SIZE))
		{
            Len = 0;
		}
		else
		{
		    Len = ReadProRecordData( 0x01, &DLT698RecordPara, (APPData[Ch].APPMaxBufLen.w- APPData[Ch].APPCurrentBufLen.w-2), &APPData[Ch].pAPPBuf[APPData[Ch].APPCurrentBufLen.w+2]);
		}

		eResultChoice = DealGetRequestRecordResult( Ch, Len );
		if( eResultChoice != eADD_OK )
		{
			return eResultChoice;
		}
	}
	else//添加DAR
	{
        eResultChoice = DLT698AddBufDAR( Ch, DAR );
        if( eResultChoice != eADD_OK )
        {
            return eResultChoice;
        }
	}
		
	APPData[Ch].BufDealLen.w += (4+1+4+DataLen+LenOffest+TmpLen.w*5);// 添加已处理下行buf长度4 oad+1方法 +4 oad + 8 时间 + 1rcsd个数 + rcsd

	return eResultChoice;
}

//--------------------------------------------------
//功能描述:    方法2读取记录
//         
//参数:      BYTE Ch[in]                  //通道选择
//         
//         BYTE *pRSD[in]               //RSD指针
//         
//         WORD OI[in]                  //读取的记录OI
//         
//         BYTE ClassAttribute[in]      //读取的属性
//         
//返回值:     eAPPBufResultChoice          //添加BUF操作结果
//         
//备注内容:    无
//--------------------------------------------------
eAPPBufResultChoice GetRequestRecordDataMethod2(BYTE Ch, BYTE *pRSD,WORD OI,BYTE ClassAttribute)
{
	BYTE *pRCSD,DAR,LenOffest;
	BYTE StartTimeBuf[7],EndTimeBuf[7];
	BYTE Buf[(MAX_ATTRIBUTE+10)*4+20];
	WORD Len,Result,DataLen,DataLen1,DataLen2,TotalLen;
	DWORD AbsStartTime698,AbsEndTime698;
	BYTE TIIndex;
	TTwoByteUnion TIData,TmpLen,Num;
	WORD NumAddress;
	eAPPBufResultChoice eResultChoice;
    TDLT698RecordPara DLT698RecordPara;

	DAR = DAR_Success;
	DLT698RecordPara.TimeIntervalPara.AllCycle = 0;
	TIIndex = 0;
	AbsStartTime698 = 0;
	AbsEndTime698 = 0;
	
	Result = DealRSDData( pRSD, ( TDLT698RecordPara * )&DLT698RecordPara);
	if( Result == FALSE )
	{
		DAR = DAR_WrongType;
	}
 
    DataLen = GetProtocolDataLen( pRSD+5 );//获取起始值长度
    if( DataLen == 0x8000 )//如果获取长度失败，暂时定为程序无应答
    {
        return eAPP_ERR_RESPONSE;
    }

    DataLen1 = GetProtocolDataLen( pRSD+5+DataLen );//获取结束值长度
    if( DataLen1 == 0x8000 )//如果获取长度失败，暂时定为程序无应答
    {
        return eAPP_ERR_RESPONSE;
    }

    DataLen2 = GetProtocolDataLen( pRSD+(5+DataLen+DataLen1) );//获取数据间隔
    if( DataLen2 == 0x8000 )//如果获取长度失败，暂时定为程序无应答
    {
        return eAPP_ERR_RESPONSE;
    }

    TotalLen = DataLen+DataLen1+DataLen2;//起始值长度+结束值长度
    pRCSD = pRSD+5+TotalLen;
    
    if( pRSD[(5+DataLen+DataLen1)] == TI_698 )//选择ti
    {
    	TIIndex = pRSD[6+DataLen+DataLen1];//取偏移
    	TIData.b[1] =  pRSD[7+DataLen+DataLen1];
    	TIData.b[0] =  pRSD[8+DataLen+DataLen1];
    }
    else if( pRSD[(5+DataLen+DataLen1)] == NULL_698 )
    {
        DLT698RecordPara.TimeIntervalPara.AllCycle = 0x55;
    }
    else
    {
        DAR = DAR_WrongType;
    }

    //取起始时间
	if( pRSD[5] == DateTime_S_698 )//下发的时间格式必须是DateTime_S_698
	{
		memcpy(StartTimeBuf,pRSD+6,7);
		lib_InverseData(StartTimeBuf, 2);
		if( (OI >= 0x5000) && (OI <= 0x5011) )//冻结下发为分钟数
		{
            AbsStartTime698 = api_CalcInTimeRelativeMin((TRealTimer *)StartTimeBuf);//计算起始时间
            if( AbsStartTime698 == ILLEGAL_VALUE_8F )
            {
                DAR = DAR_OverRegion;
            }
            else if( StartTimeBuf[6] != 0 )//秒不为零时，下发的相对分钟数加1分钟，避免时间对应不上的问题
    		{
                AbsStartTime698 += 1;
    		}
		}
		else//事件下发为秒
		{
            AbsStartTime698 = api_CalcInTimeRelativeSec((TRealTimer *)StartTimeBuf);//计算起始时间
            if( AbsStartTime698 == ILLEGAL_VALUE_8F )
            {
                DAR = DAR_OverRegion;
            }
		}
	}
	else
	{
		DAR = DAR_WrongType;
	}
	
	//取结束时间
	if( pRSD[13] == DateTime_S_698 )//下发的时间格式必须是DateTime_S_698
	{
		memcpy(EndTimeBuf,pRSD+14,7);
		lib_InverseData(EndTimeBuf, 2);
		if( (OI >= 0x5000) && (OI <= 0x5011) )//冻结下发为分钟数
		{
            AbsEndTime698 = api_CalcInTimeRelativeMin((TRealTimer *)EndTimeBuf);//计算起始时间
            if( AbsEndTime698 == ILLEGAL_VALUE_8F )
            {
                DAR = DAR_OverRegion;
            }
            else if( EndTimeBuf[6] != 0 )//秒不为零时，下发的相对分钟数加1分钟，避免时间对应不上的问题
    		{
                AbsEndTime698 += 1;
    		}
		}
		else//事件下发为秒
		{
            AbsEndTime698 = api_CalcInTimeRelativeSec((TRealTimer *)EndTimeBuf);//计算起始时间
            if( AbsEndTime698 == ILLEGAL_VALUE_8F )
            {
                DAR = DAR_OverRegion;
            }
		}
	}
	else
	{
		DAR = DAR_WrongType;
	}
	
	if( ((OI >= 0x5002) && (OI <= 0x5006)) && ( OI != 0x5005) )//其他的冻结不管时间间隔
	{
	    if( DLT698RecordPara.TimeIntervalPara.AllCycle != 0x55 )
	    {
            if( OI == 0x5006)//月冻结时间间隔单独判断
            {
                if( TIIndex != 4 )
                {
                    DAR =DAR_RefuseOp;
                }       
            }         
            else if( (OI-0x5001) != TIIndex )//如果时间间隔单位不对，返回错误
            {
                DAR =DAR_RefuseOp;
            }
	    }
	}

    if( TIData.w == 0 )//如果时间间隔为零代表全时间段
    {
        DLT698RecordPara.TimeIntervalPara.AllCycle = 0x55;
    }
    
	//RCSD
	LenOffest =  Deal_698DataLenth( pRCSD, TmpLen.b, ePROTOCOL_TYPE );
	eResultChoice = DealRCSDData( Ch, OI, pRCSD,Buf, (BYTE *)&DAR);
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
    
    if( APPData[Ch].TimeTagFlag == eTime_Invalid )//时间标签错误
    {
        DAR = DAR_TimeStamp;
    }
	else if( JudgeTaskAuthority( Ch, eREAD_MODE, pRSD-4) == FALSE )//判断安全模式参数
	{			
		DAR = DAR_PassWord;
	}
	else if( JudgeRecordOAD(OI,ClassAttribute) == FALSE )//判断记录OAD
	{			
		DAR = DAR_WrongType;
	}
	else
	{
        DLT698RecordPara.OI = OI;                                       //OI数据
        DLT698RecordPara.Ch = Ch;
        DLT698RecordPara.pOAD = Buf;                                    //pOAD指针
        DLT698RecordPara.OADNum = TmpLen.w;                             //OAD个数
        DLT698RecordPara.eTimeOrLastFlag += eRECORD_TIME_INTERVAL_FLAG;  //时间区间标志
        DLT698RecordPara.TimeOrLast = 0;                                //绝对时间数
        DLT698RecordPara.TimeIntervalPara.StartTime = AbsStartTime698;  //开始时间
        DLT698RecordPara.TimeIntervalPara.EndTime = AbsEndTime698;      //结束时间
        DLT698RecordPara.TimeIntervalPara.TI = TIData.w;                //时间间隔
        
        Result = RecordJudgePhase( OI, ClassAttribute, &DLT698RecordPara);
        if( Result == FALSE )
        {
            DAR = DAR_WrongType;
        }
	}
	
	if( DAR == DAR_Success )//无错误情况进行读取记录
	{
		if( (APPData[Ch].APPMaxBufLen.w <= (APPData[Ch].APPCurrentBufLen.w+2)) //进行极限值判断
	    ||(APPData[Ch].APPMaxBufLen.w > MAX_APDU_SIZE))
		{
            Len = 0;
		}
		else
		{
            //调用函数 读取记录 返回数据
            Len = ReadProRecordData( 0x01, &DLT698RecordPara, (APPData[Ch].APPMaxBufLen.w- APPData[Ch].APPCurrentBufLen.w-2), &APPData[Ch].pAPPBuf[APPData[Ch].APPCurrentBufLen.w+2]);      
		}
		
        NumAddress = APPData[Ch].APPCurrentBufLen.w+2;

		eResultChoice = DealGetRequestRecordResult( Ch, Len );//添加数据头  即使第一条是结束，但是通过Len=0x8000进行结果处理
		if( eResultChoice != eADD_OK )
		{
			return eResultChoice;
		}
		
        Num.w = DLT698RecordPara.TimeIntervalPara.ReturnFreezeNum;
        if( Len < 0x8000 )
        {
            Deal_698SequenceOfNumData( Ch, NumAddress, Num.b);//对记录条数进行处理
        }
	}
	else//添加DAR
	{
        eResultChoice = DLT698AddBufDAR( Ch, DAR );
        if( eResultChoice != eADD_OK )
        {
            return eResultChoice;
        }
	}
	
	APPData[Ch].BufDealLen.w += (4+1+4+DataLen+DataLen1+DataLen2+LenOffest+TmpLen.w*5);// 添加已处理下行buf长度 4 oad + 1方法 +  4 oad + 8 时间 + 8 时间 +4 时间间隔 +  1rcsd个数+ rcsd

	return eResultChoice;
}

//--------------------------------------------------
//功能描述:    方法9读取记录
//         
//参数:      BYTE Ch[in]              //通道
//         
//         BYTE *pRSD[in]           //RSD指针
//         
//         WORD OI[in]              //记录OI
//         
//         BYTE ClassAttribute[in]  //读取的属性
//         
//返回值:     eAPPBufResultChoice 返回添加buf结果
//         
//备注内容:    无
//--------------------------------------------------
eAPPBufResultChoice GetRequestRecordDataMethod9(BYTE Ch, BYTE *pRSD,WORD OI,BYTE ClassAttribute)
{
	BYTE LenOffest,DAR;
	BYTE Buf[(MAX_ATTRIBUTE+10)*4+20];
	WORD Len;
	WORD No,Result;
	TTwoByteUnion TmpLen;
	BYTE *pRCSD;
	eAPPBufResultChoice eResultChoice;
    TDLT698RecordPara DLT698RecordPara;
	
	No =1;
	DAR = DAR_Success;
	pRCSD = pRSD+2;
	No = pRSD[1];//获取读取上几次
		
	//RCSD
	LenOffest =  Deal_698DataLenth( pRCSD, TmpLen.b, ePROTOCOL_TYPE ); 
	eResultChoice = DealRCSDData( Ch, OI, pRCSD,Buf, (BYTE *)&DAR);
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	
	if( APPData[Ch].TimeTagFlag == eTime_Invalid )//时间标签错误
	{
		DAR = DAR_TimeStamp;
	}
	else if( JudgeTaskAuthority( Ch, eREAD_MODE, pRSD-4) == FALSE )//判断安全模式参数
	{			
		DAR = DAR_PassWord;
	}
	else if( JudgeRecordOAD(OI,ClassAttribute) == FALSE )//判断记录OAD
	{			
		DAR = DAR_WrongType;
	}
	else
	{
        DLT698RecordPara.OI = OI;                   //OI数据
        DLT698RecordPara.Ch = Ch;
        DLT698RecordPara.pOAD = Buf;                //pOAD指针
        DLT698RecordPara.OADNum = TmpLen.w;     //OAD个数
        DLT698RecordPara.eTimeOrLastFlag = eNUM_FLAG;   //选择次数
        DLT698RecordPara.TimeOrLast = No;           //绝对时间数
        Result = RecordJudgePhase( OI, ClassAttribute, &DLT698RecordPara);
        if( Result == FALSE )
        {
            DAR = DAR_WrongType;
        }
	}
	
	if( DAR == DAR_Success )//无错误 读取记录
	{
		if( (APPData[Ch].APPMaxBufLen.w <= (APPData[Ch].APPCurrentBufLen.w+2)) //进行极限值判断
	    ||(APPData[Ch].APPMaxBufLen.w > MAX_APDU_SIZE))
		{
            Len = 0;
		}
		else
		{
    		//调用函数 读取记录 返回数据
    		Len = ReadProRecordData( 0x01, &DLT698RecordPara, (APPData[Ch].APPMaxBufLen.w- APPData[Ch].APPCurrentBufLen.w-2), &APPData[Ch].pAPPBuf[APPData[Ch].APPCurrentBufLen.w+2]);
		}
		
		eResultChoice = DealGetRequestRecordResult( Ch, Len );
		if( eResultChoice != eADD_OK )
		{
			return eResultChoice;
		}
	}
	else//添加DAR
	{
		eResultChoice = DLT698AddBufDAR( Ch, DAR );
		if( eResultChoice != eADD_OK )
		{
			return eResultChoice;
		}
	}
	
	APPData[Ch].BufDealLen.w += 4+2+LenOffest+TmpLen.w*5;// 添加已处理下行buf长度 4 oad + 2 方法9 + 1个数 +rcsd

	return eResultChoice;
}
//--------------------------------------------------
//功能描述:  获取当前事件是否没有结束  
//         
//参数:      OI  
//			Phase
//         
//返回值:    TRUE  结束了
//			FALSE  没结束要补
//         
//备注:    用于补冻结判断
//--------------------------------------------------
BOOL api_GetFlashLastEventEnd(WORD OI,BYTE Phase)
{
	TEventOADCommonData EventOADCommonData;
	TEventAttInfo		EventAttInfo;
	TEventDataInfo		EventDataInfo;
	TEventAddrLen		EventAddrLen;
	TEventSectorInfo	EventSectorInfo;
	DWORD dwTemp;

	if( GetEventIndex( OI, &EventAddrLen.EventIndex ) == FALSE )
	{
		return TRUE;
	}

	EventAddrLen.Phase = Phase;
	if( GetEventInfo( &EventAddrLen ) == FALSE )
	{
		return TRUE;
	}
	
	api_ReadFromContinueEEPRom(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);
	api_VReadSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo);
	
	//根据当前记录号为0，且当前状态为事件没发生
	if( EventDataInfo.RecordNo == 0 )
	{
		return TRUE;
	}

	EventDataInfo.RecordNo -= 1;	//读取上一次
	//计算此条记录偏移地址
	GetEventSectorInfo(EventAttInfo.AllDataLen, EventAttInfo.MaxRecordNo, EventDataInfo.RecordNo, &EventSectorInfo);
	
	dwTemp = EventAddrLen.dwRecordAddr + EventSectorInfo.dwSectorAddr;
	
	api_ReadMemRecordData( dwTemp, sizeof(TEventOADCommonData), (BYTE *)&EventOADCommonData );
	if (lib_IsAllAssignNum( (BYTE *)&EventOADCommonData.EventTime.EndTime, 0xff, sizeof(TRealTimer) ) == TRUE)
	{
		return FALSE;
	}
	

	return TRUE;
}
//--------------------------------------------------
//功能描述:    读取冻结类数据表
//参数:      BYTE Ch[in]          通道选择
//         
//         BYTE *pData[in]      Data指针
//         
//返回值:     eAPPBufResultChoice  返回添加buf结果
//         
//备注内容:    无
//--------------------------------------------------
eAPPBufResultChoice GetRequestRecordDataTable(BYTE Ch, BYTE *pData )
{
	TTwoByteUnion OI;
	BYTE *pRSD, ClassAttribute;
	eAPPBufResultChoice eResultChoice;
	BYTE Outdata[2048];
	BOOL bNext=FALSE;
	WORD Len,Firstoffset;
	DWORD OAD=0;

	eResultChoice = eAPP_ERR_RESPONSE;
	lib_ExchangeData((BYTE*)&OAD,pData,4);//获取冻结OI
	lib_ExchangeData(OI.b,pData,2);//获取冻结OI
	ClassAttribute = pData[2];
	pRSD = pData + 4;//获取RSD指针
	FindThisEventSaveOI(&OI.w,0,eEventMap);
	if(OI.w==0x6000)
	{
		Len=Class11_Get_rec(Ch,OAD,pRSD,0,&APPData[Ch].pAPPBuf[APPData[Ch].APPCurrentBufLen.w],(APPData[Ch].APPMaxBufLen.w- APPData[Ch].APPCurrentBufLen.w),&bNext,&Firstoffset);
		if (Len)
		{
			APPData[Ch].APPCurrentBufLen.w+=Len;
			eResultChoice=eADD_OK;
			if (bNext)
			{
				SetNextPara(Ch,(void*)Class11_Get_rec,OAD,Firstoffset,pData,2);
			}
			return eResultChoice;
		}
		else
		{
			// Len=Class10_Get_rec(Ch,OAD,pRSD,0,&APPData[Ch].pAPPBuf[APPData[Ch].APPCurrentBufLen.w],(APPData[Ch].APPMaxBufLen.w- APPData[Ch].APPCurrentBufLen.w), &bNext,&Firstoffset);
			// if (Len)
			// {
			// 	APPData[Ch].APPCurrentBufLen.w+=Len;
            //     eResultChoice=eADD_OK;
			// 	if (bNext)
			// 	{
			// 		SetNextPara(Ch,(void*)Class10_Get_rec,OAD,Firstoffset,pData,2);
			// 	}
			// 	return eResultChoice;
			// }
			// else
			{
				return eAPP_ERR_RESPONSE;
			}
		}
	}

	
	switch(pRSD[0])//选择方法
	{
		case 0:
			break;
			
		case 1://选择方法1
		
			eResultChoice = GetRequestRecordDataMethod1( Ch, pRSD, OI.w,ClassAttribute);
			
			break;
		case 2://选择方法2 为指定对象区间内连续间隔值 
	
			eResultChoice = GetRequestRecordDataMethod2( Ch, pRSD, OI.w,ClassAttribute);

			break;
	
		case 9://选择方法9 读取上几次
		
			eResultChoice = GetRequestRecordDataMethod9( Ch, pRSD, OI.w,ClassAttribute);
			
			break;
			
		default:
			return  eAPP_ERR_RESPONSE;
			break;
	}
	
	return eResultChoice;
}


//--------------------------------------------------
//功能描述:    读取记录主函数
//         
//参数:      BYTE Ch[in]          通道选择
//         
//         BYTE *pOAD[in]       OAD指针
//         
//返回值:     eAPPBufResultChoice  添加Buf返回结果
//         
//备注内容:    无
//--------------------------------------------------
eAPPBufResultChoice GetRequestRecord( BYTE Ch,BYTE *pOAD)
{
	eAPPBufResultChoice eResultChoice;
	
	if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return eAPP_ERR_RESPONSE;
    }

	eResultChoice = DLT698AddBuf(Ch,0, pOAD, 4);//添加OAD数据 不是完整数据
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}

	eResultChoice = GetRequestRecordDataTable(Ch, pOAD);//获取记录表
	
	return eResultChoice;
}

//--------------------------------------------------
//功能描述:    读取记录list主函数
//         
//参数:      BYTE Ch[in]          通道选择
//         
//返回值:     eAPPBufResultChoice  添加Buf返回结果 
//         
//备注内容:    无
//--------------------------------------------------
eAPPBufResultChoice GetRequestRecordList( BYTE Ch )
{
	BYTE i,LenOffest;
    WORD NoAddress;
	TTwoByteUnion Num,No;
	eAPPBufResultChoice eResultChoice;	
	
    if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return eAPP_ERR_RESPONSE;
    }
    
    LenOffest = Deal_698DataLenth( (BYTE*)&LinkData[Ch].pAPP[APPData[Ch].BufDealLen.w], Num.b, ePROTOCOL_TYPE ); 
    APPData[Ch].BufDealLen.w +=LenOffest;

	No.w = 0;
	
	eResultChoice = DLT698AddOneBuf(Ch,0, Num.b[0] );//添加个数 产生后续帧不正确
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	
    NoAddress = APPData[Ch].APPCurrentBufLen.w;

	for(i = 0; i < Num.w;i++)
	{
		eResultChoice = GetRequestRecord(Ch,&LinkData[Ch].pAPP[APPData[Ch].BufDealLen.w]);

        if( eResultChoice != eADD_OK) 
		{
			break;
		}

		No.w ++;//sequence of record 的个数加1 
		
		if( APPData[Ch].BufDealLen.w >= LinkData[Ch].pAPPLen.w )//已处理数据大于下发的数据进行返回，避免错误报文的干扰
		{
			break;
		}
	}
	
    JudgeAppBufLen(Ch);

    Deal_698SequenceOfNumData( Ch, NoAddress, No.b);
    
	return eResultChoice;
}
//-----------------------------------------------
//函数功能: 设置跟随上报模式字
//
//参数:      Buf[in]：	保存数据的缓冲
//			
//返回值:	   返回读数据长度
//			
//备注:      eAPPBufResultChoice  
//-----------------------------------------------
BYTE SetRequestFollowReportMode( BYTE *pData )
{
	BYTE DAR;
	BYTE Buf[10];

	DAR = DAR_WrongType;	
	
	if( (pData[0] != Bit_string_698)||(pData[1] != 32) )//如果数据类型不对，数据长度不对
	{
		DAR = DAR_OtherErr;		
	}
	else
	{	
		lib_ExchangeBit( Buf, (BYTE*)&pData[2], 4);
	
		if( api_SetFollowReportMode(Buf) == FALSE )
		{
			DAR = DAR_OtherErr;
		}
		else
		{
			DAR = DAR_Success;
		}
	}	
	
    return DAR;
}
//--------------------------------------------------
//功能描述:    设置事件关联对象属性表
//         
//参数:      WORD OI[in]            数据OI 
//         
//         BYTE ClassIndex[in]    元素偏移
//         
//         BYTE *pData[in]        data指针
//                 
//返回值:     BYTE  DAR结果
//         
//备注内容:    无
//--------------------------------------------------
BYTE SetEventRelationTable( WORD OI,BYTE ClassIndex,BYTE *pData )
{
	BYTE DAR,Buf[MAX_EVENT_OAD_NUM*4+30],i,OadType;
	WORD Result;
        
	Result = FALSE;
	DAR = DAR_WrongType;
	OadType = 0;
	i  = 0;
	if( ClassIndex != 0 )//设置单个属性
	{
		return DAR_RefuseOp;
	}
	
	if( pData[0] == Array_698 )
	{
	    if( pData[1] <= MAX_EVENT_OAD_NUM )
	    {
            for( i=0; i< pData[1]; i++ )
            {
                if( pData[2+5*i] == OAD_698 )
                {
                    memcpy( &Buf[4*i], &pData[2+5*i+1],0x04);
                }
                else
                {
                    break;
                }
            }

            if( i == pData[1] )//没有错误
            {
                DAR = DAR_Success;
            }
            else
            {
                DAR = DAR_WrongType;
            }
	    }
	}

	if( DAR == DAR_Success )
	{
		Result =  api_WriteEventAttribute( OI, OadType,Buf,pData[1]);
		if( Result == FALSE )
        {
            DAR = DAR_RefuseOp;//此为固定错误返回，不可随意更改，与上层调用函数有关
        }
        else
        {
            DAR = DAR_Success;
        }
	}
        
	return DAR;
}

//--------------------------------------------------
//功能描述:    设置事件关联对象属性表
//         
//参数:      BYTE *InBuf[in]            输入buf
//         
//         BYTE *OutBuf[out]          输出buf   
//
//         BYTE Len                   要转化的长度，只能是2或者4
//         
//备注内容:    无
//--------------------------------------------------
WORD DoubleLongOrLongToAbs(         BYTE *InBuf, BYTE *OutBuf, BYTE Len )
{
    TFourByteUnion LLValue;
    TTwoByteUnion LValue;

    LLValue.l = 0;
    LValue.sw = 0;

    if( (Len != 4) && (Len != 2) )
    {
        return FALSE;
    }

    if( Len == 4 )
    {
        lib_ExchangeData( LLValue.b, InBuf, Len );

        if( LLValue.l < 0 )
        {
           LLValue.l = ~LLValue.l+1;
        }

        memcpy( OutBuf, LLValue.b, Len);
    }
    else
    {
        lib_ExchangeData( LValue.b, InBuf, Len );

        if( LValue.sw < 0 )
        {
           LValue.sw = ~LValue.sw+1;
        }

        memcpy( OutBuf, LValue.b, Len);
    }

    
    return TRUE;
}

#if( MAX_PHASE != 1 )	
//--------------------------------------------------
//功能描述:    设置失压事件记录配置参数
//         
//参数:      WORD OI[in]            数据OI
//         
//         BYTE ClassIndex[in]    元素索引
//         
//         BYTE *pData[in]        DATA数据指针                
//         
//返回值:     BYTE  DAR结果
//         
//备注内容:    无
//--------------------------------------------------
BYTE SetEventLost_V( WORD OI,BYTE ClassIndex,BYTE *pData )
{
	BYTE DAR,Buf[30];
	BOOL Result;
	
	DAR = DAR_WrongType;
	
	if( ClassIndex > 4 )
	{
		return DAR ;
	}

	if( ClassIndex == 0 )//设置所有属性
	{
		if( pData[0] == Structure_698 )//结构体
		{
			if( pData[1] == 4 ) // 4个元素
			{
				if( pData[2] == Long_unsigned_698 )//电压触发下限
				{
					lib_ExchangeData(Buf, &pData[3], 2);
					if( pData[5] == Long_unsigned_698 )//电压恢复下限
					{
						lib_ExchangeData( &Buf[2],  &pData[6], 2);
						if( pData[8] == Double_long_698 )//电流触发下限
						{
							//lib_ExchangeData( &Buf[4],  &pData[9], 4);
							DoubleLongOrLongToAbs(&pData[9], &Buf[4], 4 );
							if( pData[13] == Unsigned_698 )//判定延时时间
							{
								Buf[8] = pData[14];
								DAR = DAR_Success;
								
							}
						}
					}
				}
			}		
		}

	}
	else
	{
		if( ClassIndex < 3 )
		{
			if( pData[0] == Long_unsigned_698 )// 电压触发上限 或者 电压恢复下限
			{
				lib_ExchangeData( Buf,  &pData[1], 2);
				DAR = DAR_Success;
			}
		}
		else if( ClassIndex == 3 )
		{
			if( pData[0] == Double_long_698 )//电流触发下限
			{
				//lib_ExchangeData( Buf,  &pData[3], 4);
				DoubleLongOrLongToAbs(&pData[1], Buf, 4 );
				DAR = DAR_Success;
			}
		}
		else
		{
			if( pData[0] == Unsigned_698 )//判定延时时间
			{
				Buf[0] = pData[1];
				DAR = DAR_Success;
			}
		}
	}
	
	if( DAR == DAR_Success)
	{
		Result = api_WriteEventPara(OI, ClassIndex, Buf);
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;
		}
		else
		{
			DAR = DAR_Success;
		}
	}
	
	return DAR;
}
//--------------------------------------------------
//功能描述:    设置欠压或过压事件记录配置参数
//         
//参数:      WORD OI[in]             数据OI
//         
//         BYTE ClassIndex[in]     元素索引
//         
//         BYTE *pData[in]         DATA数据指针
//                 
//返回值:     BYTE  DAR结果 
//         
//备注内容:    无
//--------------------------------------------------
BYTE SetEventWeakOrOver_V( WORD OI,BYTE ClassIndex,BYTE *pData )//欠压或者过压
{
	BYTE DAR,Buf[30];
	BOOL Result;
	
	DAR = DAR_WrongType;
	
	if( ClassIndex > 2 )
	{
		return DAR ;
	}


	if( ClassIndex == 0 )//设置所有属性
	{
		if( pData[0] == Structure_698 )//结构体
		{
			if( pData[1] == 2 ) // 4个元素
			{
				if( pData[2] == Long_unsigned_698)//电压触发上限 或下限
				{
					lib_ExchangeData( Buf,  &pData[3], 2);
					if( pData[5] == Unsigned_698 )//判定延时时间
					{
						Buf[2] = pData[6];
						DAR = DAR_Success;
					}
				}
			}		
		}
	}
	else
	{
		if( ClassIndex == 1 )
		{
			if( pData[0] == Long_unsigned_698 )//电压触发上限 或下限
			{
				lib_ExchangeData( Buf,  &pData[1], 2);
				DAR = DAR_Success;
			}
		}
		else
		{
			if( pData[0] == Unsigned_698 )//判定延时时间
			{
				Buf[0] = pData[1];
				DAR = DAR_Success;
			}
		}
	}
	
	if( DAR == DAR_Success)
	{
		Result = api_WriteEventPara( OI, ClassIndex, Buf);
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;
		}
		else
		{
			DAR = DAR_Success;
		}
	}
	
	return DAR;
}
//--------------------------------------------------
//功能描述:    设置断相或断流事件记录配置参数
//         
//参数:      WORD OI[in]             数据OI
//         
//         BYTE ClassIndex[in]     元素索引
//         
//         BYTE *pData[in]         DATA数据指针
//                  
//返回值:     BYTE  DAR结果 
//         
//备注内容:    无
//--------------------------------------------------
BYTE SetEventBreakOrBreak_I( WORD OI,BYTE ClassIndex,BYTE *pData )//断相或者断流
{
	BYTE DAR,Buf[30];
	BOOL Result;
	
	DAR = DAR_WrongType;
	
	if( ClassIndex > 3 )//断相、断流有3个参数，电压触发上限, 电流触发上限,判定延时时间 wlk
	{
		return DAR;
	}

	if( ClassIndex == 0 )//设置所有属性
	{
		if( pData[0] == Structure_698 )//结构体
		{
			if( pData[1] == 3 ) // 4个元素
			{
				if( pData[2] == Long_unsigned_698)//电压触发上限 或下限
				{
					lib_ExchangeData( Buf,  &pData[3], 2);
					if( pData[5] == Double_long_698 )//电流触发上下限
					{
						//lib_ExchangeData( &Buf[2],  &pData[6], 4);
						DoubleLongOrLongToAbs(&pData[6], &Buf[2], 4);
						if( pData[10] == Unsigned_698 )//判定延时时间
						{
							Buf[6] = pData[11];
							DAR = DAR_Success;
						}
					}
				}
			}
		}
	}
	else
	{
		if( ClassIndex == 1 )
		{
			if( pData[0] == Long_unsigned_698 )//电压触发上限 或下限
			{
				lib_ExchangeData( Buf,  &pData[1], 2);
				DAR = DAR_Success;
			}
		}
		else if( ClassIndex == 2 ) 
		{
			if( pData[0] == Double_long_698 )//电流触发上下限
			{
				//lib_ExchangeData( Buf,  &pData[3], 4);
				DoubleLongOrLongToAbs(&pData[1], Buf, 4);
				DAR = DAR_Success;
			}
		}
		else
		{
			if( pData[0] == Unsigned_698 )//判定延时时间
			{
				Buf[0] = pData[1];
				DAR = DAR_Success;
			}
		}
	}
	
	if( DAR == DAR_Success)
	{
		Result = api_WriteEventPara( OI, ClassIndex, Buf);
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;
		}
		else
		{
			DAR = DAR_Success;
		}
	}

	return DAR;
}
//--------------------------------------------------
//功能描述:    设置失流事件记录配置参数
//         
//参数:      WORD OI[in]             数据OI
//         
//         BYTE ClassIndex[in]     元素索引
//         
//         BYTE *pData[in]         DATA数据指针
//                 
//返回值:     BYTE  DAR结果     
//         
//备注内容:    无
//--------------------------------------------------
BYTE SetEventLost_I( WORD OI,BYTE ClassIndex,BYTE *pData )//失流
{
	BYTE DAR,Buf[30];
	BOOL Result;
	
	DAR = DAR_WrongType;
	
	if( ClassIndex > 4 )
	{
		return DAR ;
	}
	
	if( ClassIndex == 0 )//设置所有属性
	{
		if( pData[0] == Structure_698 )//结构体
		{
			if( pData[1] == 4 ) // 4个元素
			{
				if( pData[2] == Long_unsigned_698 )//电压触发下限
				{
					lib_ExchangeData(Buf,  &pData[3], 2);
					if( pData[5] == Double_long_698)//电压恢复下限
					{
						//lib_ExchangeData( &Buf[2],  &pData[6], 4);
						DoubleLongOrLongToAbs(&pData[6], &Buf[2], 4);
						if( pData[10] == Double_long_698 )//电流触发下限
						{
							//lib_ExchangeData( &Buf[6],  &pData[11], 4);
							DoubleLongOrLongToAbs(&pData[11], &Buf[6], 4);
							if( pData[15] == Unsigned_698 )//判定延时时间
							{
								Buf[10] = pData[16];
								DAR = DAR_Success;
							}
						}
					}
				}
			}		
		}
	}
	else
	{
		if( ClassIndex == 1 )
		{
			if( pData[0] == Long_unsigned_698 )// 电压触发上限 或者 电压恢复下限
			{
				lib_ExchangeData( Buf,  &pData[1], 2);
				DAR = DAR_Success;
			}
		}
		else if( ClassIndex < 4 )
		{
			if( pData[0] == Double_long_698 )//电流触发下限
			{
				//lib_ExchangeData( Buf,  &pData[3], 4);
				DoubleLongOrLongToAbs(&pData[1], Buf, 4);
				DAR = DAR_Success;
			}
		}
		else
		{
			if( pData[0] == Unsigned_698 )//判定延时时间
			{
				Buf[0] = pData[1];
				DAR = DAR_Success;
			}
		}
	}
	
	if( DAR == DAR_Success)
	{
		Result = api_WriteEventPara( OI, ClassIndex, Buf);
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;
		}
		else
		{
			DAR = DAR_Success;
		}
	}

	return DAR;
}
//--------------------------------------------------
//功能描述:    设置过流、潮流反向、过载事件记录的配置参数
//         
//参数:      WORD OI[in]             数据OI
//         
//         BYTE ClassIndex[in]     元素索引
//         
//         BYTE *pData[in]         DATA数据指针
//                  
//返回值:     BYTE  DAR结果  
//         
//备注内容:    无
//--------------------------------------------------
BYTE SetEventDemandOver( WORD OI,BYTE ClassIndex,BYTE *pData )//过流 潮流反向 过载
{
	BYTE DAR,Buf[30];
	BOOL Result;
	
	DAR = DAR_WrongType;
	
	if( ClassIndex > 2 )
	{
		return DAR ;
	}
	
	if( ClassIndex == 0 )//设置所有属性
	{
		if( pData[0] == Structure_698 )//结构体
		{
			if( pData[1] == 2 ) // 2个元素
			{
				if( pData[2] == Double_long_unsigned_698)//电压触发上限 或下限
				{
					lib_ExchangeData( Buf,  &pData[3], 4);
					if( pData[7] == Unsigned_698 )//判定延时时间
					{
						Buf[4] = pData[8];
						DAR = DAR_Success;
					}
				}
			}	
		}
	}
	else
	{
		if( ClassIndex == 1 )
		{
			if( pData[0] == Double_long_unsigned_698 )//电压触发上限 或下限
			{
				lib_ExchangeData( Buf,  &pData[1], 4);
				DAR = DAR_Success;
			}
		}
		else
		{
			if( pData[0] == Unsigned_698 )//判定延时时间
			{
				Buf[0] = pData[1];
				DAR = DAR_Success;
			}
		}
	}
	
	if( DAR == DAR_Success)
	{
		Result = api_WriteEventPara( OI, ClassIndex, Buf);
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;
		}
		else
		{
			DAR = DAR_Success;
		}
	}

	return DAR;
}

//--------------------------------------------------
//功能描述:    设置互感器线圈变更事件记录配置参数
//         
//参数:      WORD OI[in]            数据OI
//         
//         BYTE ClassIndex[in]    元素索引
//         
//         BYTE *pData[in]        DATA数据指针                
//         
//返回值:     BYTE  DAR结果
//         
//备注内容:    无
//--------------------------------------------------
BYTE SetEventRogowski_Change( WORD OI,BYTE ClassIndex,BYTE *pData )
{
	BYTE DAR,Buf[30];
	BOOL Result;
	
	DAR = DAR_WrongType;
	
	if( ClassIndex > 2 )
	{
		return DAR ;
	}

	if( ClassIndex == 0 )//设置所有属性
	{
		if( pData[0] == Structure_698 )//结构体
		{
			if( pData[1] == 2 ) // 2个元素
			{
				if( pData[2] == Long_unsigned_698 )//接入判定延时
				{
					lib_ExchangeData(Buf, &pData[3], 2);
					if( pData[5] == Long_unsigned_698 )//断开判定延时
					{
						lib_ExchangeData( &Buf[2],  &pData[6], 2);
						DAR = DAR_Success;
					}
				}
			}		
		}

	}
	else
	{
		if( pData[0] == Long_unsigned_698 )// 接入判定延时 或者 断开判定延时
		{
			lib_ExchangeData( Buf,  &pData[1], 2);
			DAR = DAR_Success;
		}
	}
	
	if( DAR == DAR_Success)
	{
		Result = api_WriteEventPara(OI, ClassIndex, Buf);
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;
		}
		else
		{
			DAR = DAR_Success;
		}
	}
	
	return DAR;
}

//--------------------------------------------------
//功能描述:    设置终端抄表失败事件记录配置参数
//         
//参数:      WORD OI[in]            数据OI
//         
//         BYTE ClassIndex[in]    元素索引
//         
//         BYTE *pData[in]        DATA数据指针                
//         
//返回值:     BYTE  DAR结果
//         
//备注内容:    无
//--------------------------------------------------
BYTE SetEventRogowskiComm_Fail( WORD OI,BYTE ClassIndex,BYTE *pData )
{
	BYTE DAR,Buf[30];
	BOOL Result;
	
	DAR = DAR_WrongType;
	
	if( ClassIndex > 2 )
	{
		return DAR ;
	}

	if( ClassIndex == 0 )//设置所有属性
	{
		if( pData[0] == Structure_698 )//结构体
		{
			if( pData[1] == 2 ) // 2个元素
			{
				if( pData[2] == Unsigned_698 )//重试次数
				{
					Buf[0] = pData[3];
					if( (pData[4] == Unsigned_698) || (pData[4] == NULL_698) )//关联采集任务号 不取数据
					{
						// Buf[1] = pData[5];
						DAR = DAR_Success;
					}
				}
			}		
		}

	}
	else
	{
		if( (pData[0] == Unsigned_698) && ( ClassIndex == 1 ) )
		{
			Buf[0] = pData[1];
			DAR = DAR_Success;
		}
		else if( ((pData[4] == Unsigned_698) || (pData[4] == NULL_698)) && ( ClassIndex == 2 ) )//关联采集任务号 不取数据
		{
			DAR = DAR_Success;
		}

	}
	
	if( DAR == DAR_Success)
	{
		if(ClassIndex == 2)
		{
			Result = TRUE;
		}
		else
		{	
			Result = api_WriteEventPara(OI, 1, Buf); //只写第一个元素数据
		}
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;
		}
		else
		{
			DAR = DAR_Success;
		}
	}
	
	return DAR;
}
//--------------------------------------------------
//功能描述:    符合long_unsigned结构的所有事件记录的配置参数
//         
//参数:      WORD OI[in]             数据OI
//         
//         BYTE ClassIndex[in]   元素索引
//         
//         BYTE *pData[in]         DATA数据指针
//                 
//返回值:     BYTE  DAR结果    
//         
//备注内容:    无
//--------------------------------------------------
BYTE SetEventLong_Unsigned_Para( WORD OI,BYTE ClassIndex,BYTE *pData )
{
	BYTE DAR,Buf[30];
	BOOL Result;
	
	DAR = DAR_WrongType;
	
	if( ClassIndex > 2 )
	{
		return DAR ;
	}
	

	if( ClassIndex == 0 )//设置所有属性
	{
		if( pData[0] == Structure_698 )//结构体
		{
			if( pData[1] == 2 ) // 2个元素
			{
				if( pData[2] == Long_698)//电压触发上限 或下限
				{
					//lib_ExchangeData( Buf,  &pData[3], 2);
					DoubleLongOrLongToAbs(&pData[3], Buf, 2);
					if( pData[5] == Unsigned_698 )//判定延时时间
					{
						Buf[2] = pData[6];
						DAR = DAR_Success;
					}
				}
			}		
		}
	}
	else
	{
		if( ClassIndex == 1 )
		{
			if( pData[0] == Long_698 )//电压触发上限 或下限
			{
				//lib_ExchangeData( Buf,  &pData[3], 2);
				DoubleLongOrLongToAbs(&pData[1], Buf, 2);
				DAR = DAR_Success;
			}
		}
		else
		{
			if( pData[0] == Unsigned_698 )//判定延时时间
			{
				Buf[0] = pData[1];
				DAR = DAR_Success;
			}
		}
	}
	
	if( DAR == DAR_Success)
	{
		Result = api_WriteEventPara( OI, ClassIndex, Buf);
		
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;
		}
		else
		{
			DAR = DAR_Success;
		}
	}
	
	return DAR;
}
#endif//if( MAX_PHASE != 1 )
//--------------------------------------------------
//功能描述:    符合long_unsigned结构的所有事件记录的配置参数
//         
//参数:      WORD OI[in]             数据OI
//         
//         BYTE ClassIndex[in]   元素索引
//         
//         BYTE *pData[in]         DATA数据指针
//                 
//返回值:     BYTE  DAR结果    
//         
//备注内容:    无
//--------------------------------------------------
BYTE SetEventDouble_Long_Unsigned_Para( WORD OI,BYTE ClassIndex,BYTE *pData )
{
	BYTE DAR,Buf[30];
	BOOL Result;
	
	DAR = DAR_WrongType;
	
	if( ClassIndex > 3 )//零线电流异常有3个参数
	{
		return DAR;
	}

	if( ClassIndex == 0 )//设置所有属性
	{
		if( pData[0] == Structure_698 )//结构体
		{
			if( pData[1] == 3 ) // 3个元素
			{
				if( pData[2] == Double_long_698)
				{
					DoubleLongOrLongToAbs(&pData[3], &Buf[0], 4);
					//lib_ExchangeData( Buf,  &pData[3], 4);
					if( pData[7] == Long_unsigned_698 )
					{
						lib_ExchangeData( &Buf[4],  &pData[8], 2);
						//DoubleLongOrLongToAbs(&pData[8], &Buf[4], 2);
						if( pData[10] == Unsigned_698 )
						{
							Buf[6] = pData[11];
							DAR = DAR_Success;
						}
					}
				}
			}
		}
	}
	else
	{
		if( ClassIndex == 1 )
		{
			if( pData[0] == Double_long_698 )//电压触发上限 或下限
			{
				DoubleLongOrLongToAbs( &pData[1], Buf , 4);
				DAR = DAR_Success;
			}
		}
		else if( ClassIndex == 2 ) 
		{
			if( pData[0] == Long_unsigned_698 )//电流触发上下限
			{
				lib_ExchangeData( Buf,  &pData[1], 2);
				//DoubleLongOrLongToAbs(&pData[1], Buf, 2);
				DAR = DAR_Success;
			}
		}
		else
		{
			if( pData[0] == Unsigned_698 )//判定延时时间
			{
				Buf[0] = pData[1];
				DAR = DAR_Success;
			}
		}
	}
	
	if( DAR == DAR_Success)
	{
		Result = api_WriteEventPara( OI, ClassIndex, Buf);
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;
		}
		else
		{
			DAR = DAR_Success;
		}
	}

	return DAR;
}

//--------------------------------------------------
//功能描述:    符合Double_unsigned的所有事件记录的配置参数
//         
//参数:      WORD OI[in]             数据OI
//         
//         BYTE ClassIndex[in]     元素索引
//         
//         BYTE *pData[in]         DATA数据指针
//                  
//返回值:     BYTE  DAR结果   
//         
//备注内容:    无
//--------------------------------------------------
BYTE SetEvent_Double_unsigned_Para( WORD OI,BYTE ClassIndex,BYTE *pData )//过流 潮流反向 过载
{
	BYTE DAR,Buf[30];
	BOOL Result;
	
	DAR = DAR_WrongType;
	
	if( ClassIndex > 2 )
	{
		return DAR ;
	}
		
	if( ClassIndex == 0 )//设置所有属性
	{
		if( pData[0] == Structure_698 )//结构体
		{
			if( pData[1] == 2 ) // 2个元素
			{
				if( pData[2] == Double_long_698)//电压触发上限 或下限
				{
					//lib_ExchangeData( Buf,  &pData[3], 4);
					DoubleLongOrLongToAbs(&pData[3], Buf, 4);
					if( pData[7] == Unsigned_698 )//判定延时时间
					{
						Buf[4] = pData[8];
						DAR = DAR_Success;
					}
				}
			}	
		}
	}
	else
	{
		if( ClassIndex == 1 )
		{
			if( pData[0] == Double_long_698 )//电压触发上限 或下限
			{
				//lib_ExchangeData( Buf,  &pData[3], 4);
			    DoubleLongOrLongToAbs(&pData[1], Buf, 4);
				DAR = DAR_Success;
			}
		}
		else
		{
			if( pData[0] == Unsigned_698 )//判定延时时间
			{
				Buf[0] = pData[1];
				DAR = DAR_Success;
			}
		}
	}
	
	if( DAR == DAR_Success)
	{
		Result = api_WriteEventPara( OI, ClassIndex, Buf);
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;
		}
		else
		{
			DAR = DAR_Success;
		}
	}

	return DAR;
}
//--------------------------------------------------
//功能描述:    符合unsigned结构的所有事件记录的配置参数
//         
//参数:      WORD OI[in]             数据OI
//         
//         BYTE ClassIndex[in]   元素索引
//         
//         BYTE *pData[in]         DATA数据指针
//                  
//返回值:     BYTE  DAR结果     
//         
//备注内容:    无
//--------------------------------------------------
BYTE SetEventUnsignedUnion( WORD OI,BYTE ClassIndex,BYTE *pData )//辅助电源掉电  电压逆相序  电流逆相序  掉电事件
{
	BYTE DAR,Buf[30];
	BOOL Result;
	
	DAR = DAR_WrongType;
	if( (ClassIndex ==0) || (ClassIndex == 1) )//clasindexes 必须为0或者1
	{
		if( ClassIndex != 0 )
		{
			pData = DealStructureClassIndexNotZeroData(pData);
		}	
		if( pData[0] == Structure_698 )
		{
			if( pData[1] == 1 )
			{
				if( pData[2] == Unsigned_698 )
				{
					Buf[0] = pData[3];
					DAR = DAR_Success;
				}
			}
		}
	}
	
	if( DAR == DAR_Success)
	{
		Result = api_WriteEventPara( OI, ClassIndex, Buf);

		if( Result == FALSE )
		{
			DAR = DAR_HardWare;
		}
		else
		{
			DAR = DAR_Success;
		}
	}
	
	return DAR;
}
//--------------------------------------------------
//功能描述:  设置事件配置参数
//         
//参数:      WORD Type[in]           数据OI
//         
//         BYTE ClassIndex[in]     元素索引
//         
//         BYTE *pData[in]         DATA数据指针
//                 
//返回值:     BYTE  DAR结果    
//         
//备注内容:  无
//--------------------------------------------------
BYTE SetEventConfigPara( WORD Type,BYTE ClassIndex,BYTE *pData )
{	
	BYTE DAR;
        
    DAR = DAR_WrongType;
        
	switch( Type )
	{
#if( MAX_PHASE != 1 )	
	case 0x3000://电能表失压事件
		DAR = SetEventLost_V( Type,ClassIndex, pData);
		break;

	case 0x3001://电能表欠压事件
	case 0x3002://电能表过压事件
		DAR = SetEventWeakOrOver_V( Type, ClassIndex, pData);
		break;

	case 0x3003://电能表断相事件
	case 0x3006://电能表断流事件
		DAR = SetEventBreakOrBreak_I( Type, ClassIndex, pData);
		break;

	case 0x3004://电能表失流事件
		DAR = SetEventLost_I( Type,ClassIndex, pData);
		break;
		
	case 0x3007://电能表功率反向事件
	case 0x3008://电能表过载事件
		DAR = SetEvent_Double_unsigned_Para( Type,ClassIndex, pData);
		break;
		
	case 0x3009://电能表正向有功需量超限事件
	case 0x300a://电能表反向有功需量超限事件
	case 0x300b://电能表无功需量超限事件
		DAR = SetEventDemandOver( Type, ClassIndex, pData);
		break;
		
	case 0x301D://电能表电压不平衡事件
	case 0x301E://电能表电流不平衡事件
	case 0x303B://电能表功率因数超下限事件
	case 0x302D://电能表电流严重不平衡
		DAR = SetEventLong_Unsigned_Para( Type, ClassIndex, pData);
		break;
	case 0x3053://互感器线圈变更事件
		DAR = SetEventRogowski_Change( Type, ClassIndex, pData);
		break;
	case 0x310F://终端抄表失败事件
		DAR = SetEventRogowskiComm_Fail( Type, ClassIndex, pData);
		break;
#endif
	case 0x3005://电能表过流事件
		DAR = SetEvent_Double_unsigned_Para( Type,ClassIndex, pData);
		break;
#if( MAX_PHASE != 1 )	
	case 0x300f://电能表电压逆相序事件
	case 0x3010://电能表电流逆相序事件
	case 0x302C://电能表电源异常
	case 0x300e://电能表辅助电源掉电事件事件
	 	DAR = SetEventUnsignedUnion( Type,ClassIndex, pData);
	    break;
#endif	
	case 0x302F://电能表计量芯片故障事件
	    DAR = SetEventUnsignedUnion( Type,ClassIndex, pData);
	    break;
	case 0x3040://电能表零线电流异常事件
	    DAR = SetEventDouble_Long_Unsigned_Para( Type,ClassIndex, pData);
	    break;
		
	case 0x3011://电能表掉电事件
		DAR = DAR_RefuseOp;
		break;
		
	default:
		break;
	}
    return DAR;
}
//--------------------------------------------------
//功能描述:    设置事件上报标识
//         
//参数:      WORD OI[in]      数据OI
//         
//         BYTE *pData[in]  数据指针
//                  
//返回值:     BYTE DAR结果
//         
//备注内容:    无
//--------------------------------------------------
BYTE SetRequestEventReportMode( eReportClass ReportClass, WORD OI, BYTE *pData )
{
	BYTE DAR;
	WORD Result;
	
	DAR = DAR_WrongType;
	
	if( pData[0] == Enum_698)
	{
		Result = api_SetEventReportMode( ReportClass, OI, pData[1] );
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;
		}
		else
		{
			DAR = DAR_Success;
		}
		
	}

	return DAR;
}
//--------------------------------------------------
//功能描述:    设置事件有效标识
//         
//参数:      WORD OI[in]      数据OI
//         
//         BYTE *pData[in]  数据指针
//                          
//返回值:     BYTE DAR结果
//         
//备注内容: 无
//--------------------------------------------------
BYTE SetRequestEventValidFlag(           WORD OI, BYTE *pData )
{
	BYTE DAR;

	
	DAR = DAR_WrongType;
	
	if( pData[0] == Boolean_698 )
	{
	    if( pData[1] == 1 )//只允许设置为有效
	    {
            DAR = DAR_Success;
	    }
	    else
	    {
            DAR = DAR_RefuseOp;
	    }		
	}

	return DAR;
}
//--------------------------------------------------
//功能描述: 设置事件参数
//         
//参数:      BYTE Ch[in]   通道选择
//         
//         BYTE *pOAD[in] oad指针
//         
//返回值:     eAPPBufResultChoice  返回添加Buf结果
//         
//备注:
//--------------------------------------------------
eAPPBufResultChoice SetRequestEvent( BYTE Ch, BYTE *pOAD )
{
	BYTE DAR,ClassAttribute,ClassIndex;
	BYTE *pData;
	TTwoByteUnion OI;
	eAPPBufResultChoice eResultChoice;
	
	pData = &pOAD[4];//获取数据指针
	lib_ExchangeData(OI.b,pOAD,2);//获取OI
	ClassAttribute = (pOAD[2]&0x1f);
	ClassIndex = pOAD[3];
	DAR = DAR_WrongType; 

    if((OI.w == 0x3013) && (api_GetRunHardFlag(eRUN_HARD_COMMON_KEY) == FALSE) 
    && ( ClassAttribute == 3) )//私钥状态下不允许更改事件清零关联对象属性表
    {
        DAR = DAR_RefuseOp; 
    }
	else if( ((OI.w >=0x3000) && (OI.w <= 0x3008)) ||( OI.w == 0x300b)||( OI.w == 0x303b) ) //配置参数 之所以两处调用是因为属性不一样
	{
		switch( ClassAttribute )
		{
			case 2://关联对象属性表
				DAR = SetEventRelationTable( OI.w, ClassIndex, pData);
				break;

			case 5://配置参数
				DAR = SetEventConfigPara( OI.w,  ClassIndex, pData);
				break;

			case 11://上报标识
				DAR = SetRequestEventReportMode( eReportMode, OI.w, pData);
				break;

			case 12://有效标识
				DAR = SetRequestEventValidFlag( OI.w, pData);
				break;
			case 15://上报方式
				DAR = SetRequestEventReportMode( eReportMethod, OI.w, pData);
				break;
		}
		
	}
	else
	{
		switch( ClassAttribute )
		{
			case 3://关联对象属性表
				DAR = SetEventRelationTable( OI.w, ClassIndex, pData);
				break;

			case 6://配置参数
				DAR = SetEventConfigPara( OI.w,  ClassIndex, pData);
				break;

			case 8://上报标识
				DAR = SetRequestEventReportMode( eReportMode, OI.w, pData);
				break;

			case 9://有效标识
				DAR = SetRequestEventValidFlag( OI.w, pData);
				break;
			case 11://上报方式
				DAR = SetRequestEventReportMode( eReportMethod, OI.w, pData);
				break;
		}
	}

	
	eResultChoice = DLT698AddOneBuf(Ch,0x55, DAR);//是完整数据
    return eResultChoice;
}
//--------------------------------------------------
//功能描述:    设置冻结关联属性
//         
//参数:      WORD Type[in]		    冻结类型OI
//         
//         BYTE ClassAttribute[in]	冻结属性
//         
//         BYTE ClassIndex[in]	    冻结元素索引
//         
//         BYTE *pData[in]          数据指针
//                  
//返回值:     BYTE  DAR结果 
//         
//备注内容:  无
//--------------------------------------------------
BYTE SetFreezeRelationTable( WORD Type,BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )
{
	BYTE DAR,Buf[MAX_FREEZE_ATTRIBUTE*8+30],i,OadType;		//最大的可设置的关联对象属性个数是54个，应留出足够的空间避免存在超内存现象--ml
    WORD Result;
        
	Result = FALSE;
	DAR = DAR_WrongType;
	OadType = 0;
	i = 0;
	
	if( ClassAttribute != 0x03)//只能设置属性3
	{
		return DAR_Undefined;
	}
	
	if( ClassIndex != 0 )//设置单个属性
	{
		return DAR_RefuseOp;
	}
	
	if( pData[0] == Array_698 )
	{
	    if( pData[1] <= MAX_FREEZE_ATTRIBUTE )//不能超过最大关联对象个数
	    {
            for( i=0; i< pData[1]; i++ )
            {
                if( pData[2+13*i] == Structure_698 )
                {
                    if( pData[3+13*i] == 3 )
                    {
                        lib_ExchangeData( &Buf[8*i], &pData[3+2+13*i],2);//冻结周期
                        memcpy( &Buf[2+8*i], &pData[3+2+2+1+13*i],4);//OAD
                        lib_ExchangeData( &Buf[6+8*i], &pData[3+2+2+1+4+1+13*i],2);//冻结深度
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    break;
                }
            }

            if( i == pData[1] )//没有错误
            {
                DAR = DAR_Success;
            }
	    }
	    else
	    {
            DAR = DAR_OverRegion;
	    }
	}

	if( DAR == DAR_Success)
	{
		Result = api_WriteFreezeAttribue( Type, OadType,Buf, pData[1]);
		if( Result == FALSE )
		{
		    DAR = DAR_RefuseOp;
		}
		else
		{
		    DAR = DAR_Success;
		}
	}
        
    return DAR;
}
//--------------------------------------------------
//功能描述:    设置冻结属性
//         
//参数:      BYTE Ch[in]          通道选择
//         
//         BYTE *pOAD[in]       OAD数据指针
//         
//返回值:     eAPPBufResultChoice  返回添加Buf结果
//         
//备注内容:    无
//--------------------------------------------------
eAPPBufResultChoice SetRequestFreeze( BYTE Ch, BYTE *pOAD )
{
	BYTE DAR,ClassAttribute,ClassIndex;
	BYTE *pData;
	TTwoByteUnion OI;
	eAPPBufResultChoice eResultChoice;
	
	pData = &pOAD[4];//获取数据指针
	lib_ExchangeData(OI.b,pOAD,2);//获取OI
	ClassAttribute = (pOAD[2]&0x1f);
	ClassIndex = pOAD[3];
	DAR = DAR_WrongType; 
	
	DAR = SetFreezeRelationTable( OI.w, ClassAttribute, ClassIndex, pData);
	
	eResultChoice = DLT698AddOneBuf(Ch,0x55, DAR);//是完整数据
	
    return eResultChoice;
}
//--------------------------------------------------
//功能描述:    设置开关量输入标志
//         
//参数:      
//         BYTE *pOAD[in]  OAD数据指针
//         
//返回值:     eAPPBufResultChoice  返回添加Buf结果
//         
//备注内容:    无
//--------------------------------------------------
eAPPBufResultChoice SetDIInputFlag(BYTE Ch, BYTE *pOAD )
{
	BYTE Buf[20],DAR,i,ClassAttribute,ClassIndexes,bInputState,bAttributeState;
	BYTE *pData;
	WORD MaxNum,Result;
	TTwoByteUnion OI,Len;
	eAPPBufResultChoice eResultChoice;

	Result = FALSE;
	DAR = DAR_WrongType;
	pData = &pOAD[4];//获取数据指针
	lib_ExchangeData(OI.b,pOAD,2);//获取OI
	ClassAttribute = (pOAD[2]&0x1f);
	ClassIndexes = pOAD[3];
	Len.w = 0;

	switch ( ClassAttribute )
	{
		case 0x04:
			if( OI.w != 0xF203)
			{
				DAR = DAR_Undefined;
			}
			else if(ClassIndexes == 0)
			{
				if(pData[Len.w] == Structure_698)
				{
					Len.w += 1;
					if(pData[Len.w] == 2)//两个元素
					{
						Len.w += 1;
						if(pData[Len.w] == Bit_string_698)
						{
							Len.w += 2;
							lib_ExchangeBit((BYTE*)&bInputState,( pData + Len.w),1);
							if(bInputState > 1)
							{
								DAR = DAR_OverRegion;
							}
							else
							{
								Len.w += 1;
								if(pData[Len.w] == Bit_string_698)
								{
									Len.w += 2;
									lib_ExchangeBit((BYTE*)&bAttributeState,( pData + Len.w),1);	
									if(bAttributeState > 1)
									{
										DAR = DAR_OverRegion;
									}
									else
									{
										api_WriteDIPara(1,(BYTE*)&bInputState);
										api_WriteDIPara(2,(BYTE*)&bAttributeState);
										DAR = DAR_Success;
									}
								}
							}
						}
						
					}
				}
			}
			break;
		default :
			break;
	}
	
	eResultChoice = DLT698AddOneBuf(Ch,0x55, DAR);//是完整数据

	return eResultChoice;
}

//--------------------------------------------------
//功能描述:    设置LCD参数
//         
//参数:      BYTE Ch[in]     通道选择
//         
//         BYTE *pOAD[in]  OAD数据指针
//         
//返回值:     eAPPBufResultChoice  返回添加Buf结果
//         
//备注内容:    无
//--------------------------------------------------
eAPPBufResultChoice SetRequestLCD( BYTE Ch, BYTE *pOAD )
{
	BYTE Buf[20],DAR,i,ClassAttribute,ClassIndexes,ClassIndexesBak,Type,Num,LCDSequence;
	BYTE *pData;
	WORD MaxNum,Result;
	TTwoByteUnion OI,Len;
	eAPPBufResultChoice eResultChoice;

	Result = FALSE;
	DAR = DAR_WrongType;
	pData = &pOAD[4];//获取数据指针
	lib_ExchangeData(OI.b,pOAD,2);//获取OI
	ClassAttribute = (pOAD[2]&0x1f);
	ClassIndexes = pOAD[3];
	Len.w = 0;

	ClassIndexesBak = ClassIndexes;
	
	switch ( ClassAttribute )
	{
		case 2://设置显示对象列表 
			if( OI.w == 0xf300)//取最大设置屏数
			{
				MaxNum =  MAX_LCD_LOOP_ITEM;
			}
			else
			{
				MaxNum =  MAX_LCD_KEY_ITEM;
			}

			if( ClassIndexes != 0 )
			{ 
				ClassIndexes = ClassIndexes-1;
				pData = DealArrayClassIndexNotZeroData(pData);//对格式为数据的元素 当元素索引不为零时 进行数据处理
			}
			
			if( pData[1] <= MaxNum )
			{
				if( pData[0] == Array_698 )
				{
					for( i=0; i<pData[1]; i++)
					{
						if( pData[2+Len.w] == Structure_698 )
						{
							if( pData[3+Len.w] == 2 )
							{
								if( pData[4+Len.w] == CSD_698 )
								{
									if( pData[5+Len.w] == 0)//选择OAD
									{
										memcpy( Buf, &pData[6+Len.w],4);//取OAD
										Num = 1;
										LCDSequence = pData[11+Len.w];
										DAR = DAR_Success; 
										Len.w += 10;//已处理数据长度
									}
									else if( pData[5+Len.w] == 1)//选择ROAD
									{
										memcpy( Buf, &pData[6+Len.w], 4);
										memcpy( &Buf[4], &pData[11+Len.w], 4);
										
										#if(MAX_PHASE == 1)
										//日冻结(5004)以及结算日冻结(5005) 在E2有备份
										//单相低功耗下显示,由于低功耗不开flash，不配置spi，导致看门狗复位
										if( (Buf[0] == 0x50)&&((Buf[1] == 0x04)||(Buf[1] == 0x05)) )
										{}
										else
										{
											DAR = DAR_WrongType;
											break;
										}
										#endif
										
										LCDSequence = pData[11+Len.w+4+1];
                                        Num = 2;
										DAR = DAR_Success; 
										Len.w += 1+1+1+1+4+1+4+1+1;//已处理数据长度 不包括OAD
									}
									else
									{
										DAR = DAR_WrongType;
										break;
									}
									
									if( DAR == DAR_Success )
									{
										Result = api_WriteLcdItem( (OI.w -0xf300) , i+ClassIndexes, Num, Buf,LCDSequence);
										if( Result == FALSE )
										{
											DAR = DAR_HardWare;//固定返回 不可随意更改 
											break;
										}
										else
										{
											DAR = DAR_Success;
										}
									}
								}
							}
						}
					}

					if( (DAR == DAR_Success) && (ClassIndexesBak == 0) )//只有在设置全部显示项时才可以改变显示屏数
					{
                        Type = (OI.w -0xf300);
        				Result = api_WritePara(0, GET_STRUCT_ADDR( TFPara1,LCDPara.DispItemNum[0])+Type, 1, (BYTE*)&pData[1] );
        				if( Result == FALSE )
        				{
        					DAR = DAR_HardWare;
        				}
					}
				}
			}	
			break;
		
		case 3://每个对象显示时间
			if( ClassIndexes < 1 )
			{
				if( pData[0] == Long_unsigned_698 )
				{
					lib_ExchangeData( Buf, &pData[1], 2);
					DAR = DAR_Success;
				}
			}
			
			Type = (OI.w -0xf300);
			
			if( Type == 0 )//循显进行极限值判断
			{
                if( (pData[2]<5) || (pData[2]>20) )
                {
                    DAR = DAR_OverRegion;
                }
			}

            
			if( DAR == DAR_Success )
			{
				
				if( Type == 0 )
				{
					Result = api_WritePara(0, GET_STRUCT_ADDR( TFPara1,LCDPara.LoopDisplayTime), 2, Buf );
				}
				else
				{
					Result = api_WritePara(0, GET_STRUCT_ADDR( TFPara1,LCDPara.KeyDisplayTime), 2, Buf );
				}
				
				if( Result == FALSE )
				{
					DAR = DAR_HardWare;
				}
					
			}
			break;
		
		case 4://显示参数
		
			if( OI.w == 0xf300)//取最大设置屏数
			{
				MaxNum =  MAX_LCD_LOOP_ITEM;
			}
			else
			{
				MaxNum =  MAX_LCD_KEY_ITEM;
			}
			
			if( ClassIndexes < 3 )
			{
                if( ClassIndexes == 0 )
                {
                    if( pData[0] == Structure_698 )
                    {
                        if( pData[1] == 2 )
                        {
                            if( pData[2] == Unsigned_698 )
                            {
                                if( pData[4] == Unsigned_698 )
                                {
                                    if( pData[3] <= MaxNum )
                                    {
                                        Buf[0] = pData[3];
                                        DAR = DAR_Success; 
                                    }
                                    else
                                    {
                                        DAR = DAR_OverRegion;
                                    }
                                    
                                }
                            }
                        }
                    }
                }
                else if( ClassIndexes == 1 )//设置显示屏数
                {
                    if( pData[0] == Unsigned_698 )
                    {
                        if( pData[1] <= MaxNum )
                        {
                            Buf[0] = pData[1];
                            DAR = DAR_Success; 
                        }
                        else
                        {
                            DAR = DAR_OverRegion;
                        }
                    }
                }
                else
                {
                    DAR = DAR_RefuseOp;
                }
				
			}

			if( DAR == DAR_Success )
			{
				Type = (OI.w -0xf300);
				Result = api_WritePara(0, GET_STRUCT_ADDR( TFPara1,LCDPara.DispItemNum[0])+Type, 1, Buf );
				if( Result == FALSE )
				{
					DAR = DAR_HardWare;
				}
				else
				{					
					//api_ResetBorrDotNum(0);
				}
			}
			break;
		
		default :
			break;
	}

	eResultChoice = DLT698AddOneBuf(Ch,0x55, DAR);//是完整数据
	
	return eResultChoice;      
}

//--------------------------------------------------
//功能描述:    设置远程控制参数
//         
//参数:      BYTE ClassAttribute[in]   元素属性
//         
//         BYTE ClassIndex[in]       属性索引
//         
//         BYTE *pData[in]           数据指针
//                
//返回值:     BYTE DAR结果
//         
//备注内容:  无
//--------------------------------------------------
BYTE SetRemoteControlPara(           BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )
{
	BYTE DAR,Buf[10];
	WORD Result;
	
	Result = FALSE;
	DAR = DAR_WrongType;
	
	if( ClassAttribute == 2)
	{
		if( ClassIndex == 0)
		{
			if( pData[0] == Structure_698 )
			{
				if( pData[1] == 2)
				{
					if( pData[2] == Double_long_unsigned_698 )
					{
						lib_ExchangeData( Buf, &pData[3],4);
						Result = WriteRelayPara(0,Buf);
						if( Result == TRUE )
						{
							if( pData[7] == Long_unsigned_698 )
							{
								lib_ExchangeData( Buf, &pData[8],2);
								Result= WriteRelayPara(1,Buf);
								if( Result == FALSE )
								{
									DAR = DAR_HardWare; //固定返回 不可随意更改 与上层配合
								}
								else
								{
									DAR = DAR_Success; 
								}
							}
						}
						else
						{
							DAR = DAR_HardWare; //固定返回 不可随意更改 与上层配合
						}
					}
				}
			}
		}
		else if( ClassIndex == 1 )
		{
			pData = DealStructureClassIndexNotZeroData(pData);

			if( pData[0] == Structure_698 )
			{
				if( pData[1] == 1)
				{
					if( pData[2] == Double_long_unsigned_698 )
					{
						lib_ExchangeData( Buf, &pData[3],4);
						Result = WriteRelayPara(0,Buf);
						if( Result == FALSE )
						{
							DAR = DAR_HardWare; //固定返回 不可随意更改 与上层配合
						}
						else
						{
							DAR = DAR_Success; 
						}
					}
				}
			}
		}
		else if( ClassIndex == 2 )
		{
			pData = DealStructureClassIndexNotZeroData(pData);

			if( pData[0] == Structure_698 )
			{
				if( pData[1] == 1)
				{
					if( pData[2] == Long_unsigned_698 )
					{
						lib_ExchangeData( Buf, &pData[3],2);
						Result = WriteRelayPara(1,Buf);
						if( Result == FALSE )
						{
							DAR = DAR_HardWare; //固定返回 不可随意更改 与上层配合
						}
						else
						{
							DAR = DAR_Success; 
						}
					}
				}
			}
		}
	}
	
    return DAR;
}
//--------------------------------------------------
//功能描述:    设置远程控制属性
//         
//参数:      BYTE Ch[in]		通道
//         
//         BYTE *pOAD[in]	OAD指针
//         
//返回值:     eAPPBufResultChoice  返回添加Buf结果 
//         
//备注内容:    无
//--------------------------------------------------
eAPPBufResultChoice SetRequestControl( BYTE Ch, BYTE *pOAD )
{
	BYTE DAR,ClassAttribute,ClassIndex;
	BYTE *pData;
	TTwoByteUnion OI;
	eAPPBufResultChoice eResultChoice;

	DAR = DAR_WrongType;
	pData = &pOAD[4];//获取数据指针
	lib_ExchangeData(OI.b,pOAD,2);//获取OI
	ClassAttribute = (pOAD[2]&0x1f);
	ClassIndex = pOAD[3];
	
	if( OI.w == 0x8000 )//遥控
	{
		DAR = SetRemoteControlPara( ClassAttribute, ClassIndex, pData);
	}
	else//保电 暂未支持
	{
		
	}

	eResultChoice = DLT698AddOneBuf(Ch,0x55, DAR);//是完整数据
	
    return eResultChoice;
        
}

//--------------------------------------------------
//功能描述:    设置LCD参数
//         
//参数:      BYTE ClassAttribute[in]   元素属性
//         
//         BYTE ClassIndex[in]       属性索引
//         
//         BYTE *pData[in]           数据指针
//                  
//返回值:     BYTE DAR结果 
//         
//备注内容:    无
//--------------------------------------------------
BYTE SetLCDPara( BYTE ClassAttribute,BYTE ClassIndexes,BYTE *pData )
{
	BYTE Buf[20],DAR,Length;
	WORD Result;
	BYTE lcdParaOffset;

	DAR = DAR_WrongType;
	Result = FALSE;
	
	if( ClassAttribute == 2)
	{
		if( ClassIndexes == 0)
		{
			if( pData[0] == Structure_698 )
			{
				if( pData[1] == 7)
				{
					if( pData[2] == Unsigned_698 )
					{
					    if( (pData[3] < 5) ||(pData[3] > 30) )//上电全显时间范围判断
					    {
                            return DAR_OverRegion;
					    }
						Buf[0] = pData[3];//上电全显时间
						if( pData[4] == Long_unsigned_698 )
						{
							lib_ExchangeData( &Buf[1] , &pData[5], 2);//背光点亮时间
							if( pData[7] == Long_unsigned_698 )
							{
								lib_ExchangeData( &Buf[3] , &pData[8], 2);//显示查看背光点亮时间
								if( pData[10] == Long_unsigned_698 )
								{
									lib_ExchangeData( &Buf[5] , &pData[11], 2);//无电按键屏幕驻留最大时间
									if( pData[13] == Unsigned_698 )
									{
                                        if( pData[14] > 4 )//显示电能小数位数范围判断
                                        {
                                            return DAR_OverRegion;
                                        }
										Buf[7] = pData[14];//显示电能小数位数
										if( pData[15] == Unsigned_698 )
										{
											Buf[8] = pData[16];//显示功率小数位数
											if( pData[17] == Unsigned_698 )//液晶1,2字样意义
											{
												Buf[9] = pData[18];
												if( Buf[9] > 1 )//液晶①②字样意义：0显示当前套、备用套时段，1显示当前套、备用套费率。
												{
													return DAR_OverRegion;
												}
											}
											Length = 10;
											lcdParaOffset = 0;
											DAR = DAR_Success;
										}
									}			
								}
							}
						}
					}
				}
			}
		}
		else if( ClassIndexes < 8 )
		{

			pData = DealStructureClassIndexNotZeroData(pData);

			if( pData[0] == Structure_698 )
			{
				if( pData[1] == 01 )
				{
					if( (ClassIndexes >= 2) && ( ClassIndexes <= 4))
					{
						if( pData[2] == Long_unsigned_698 )
						{
							lib_ExchangeData( Buf, &pData[3], 2);//获取2个字节
							Length = 2;
							DAR = DAR_Success;
						}
						
					}
					else
					{
						if( pData[2] == Unsigned_698 )
						{
                            if( ClassIndexes == 1 )
                            {
                                if( (pData[3] < 5) ||(pData[3] > 30) )//上电全显时间范围判断
                                {
                                    return DAR_OverRegion;
                                }
                            }
                            else if( ClassIndexes == 5 )
                            {
                                if( pData[3] > 4 )//显示电能小数位数范围判断
                                {
                                    return DAR_OverRegion;
                                }
                            }
                            
							Buf[0] = pData[3];
							Length = 1;
							DAR = DAR_Success;
						}
					}

					if( ClassIndexes == 1 )
					{
						lcdParaOffset = 0;
					}
					else if( ClassIndexes < 5 )
					{
						lcdParaOffset = (1 + ( ClassIndexes-2)*2);
					}
					else
					{
						lcdParaOffset = (1 + 3*2 + (ClassIndexes - 5));
					}
				}
			}
		}
	}

	if( DAR == DAR_Success)//无错误的情况下进行设置参数
	{
		Result = api_WritePara( 0,  GET_STRUCT_ADDR(TFPara1, LCDPara.PowerOnDispTimer)+lcdParaOffset, Length, Buf);
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;//此返回结果不可随意更改，更改时需与上层进行配合
		}
		else
		{
			DAR = DAR_Success;
			//修改了电能小数位数
			if((ClassIndexes == 0) || ( ClassIndexes == 5 ))
			{
				//把电能小数位数写入借位功能电能小数位数
				//api_ResetBorrDotNum(1);
			}
		}
	}

	return DAR;
        
}
//--------------------------------------------------
//功能描述:    设置地理位置信息
//         
//参数:      BYTE ClassAttribute[in]   元素属性
//         
//         BYTE ClassIndex[in]     属性索引
//         
//         BYTE *pData[in]           数据指针
//                  
//返回值:     BYTE DAR结果  
//         
//备注内容:    无
//--------------------------------------------------
BYTE SetPsitionInfo( BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )
{
	BYTE DAR,OffestLen,Buf[20];
	WORD Result;
	
	DAR = DAR_WrongType;
	Result = FALSE;
	
	if( ClassAttribute != 0x02)//只能设置属性2
	{
		return DAR_Undefined;
	}
	Result = api_ProcMeterTypePara( READ, eMeterMeterPosition, Buf);
 	if( Result == FALSE)
	{
		return DAR_HardWare;//DAR 硬件失效
	}
	
	if( ClassIndex == 0)
	{
		if( pData[0] == Structure_698 )//结构体
		{
			if(pData[1] == 3)           //个数
			{
				if( pData[2] == Structure_698 )//经度结构体
				{
					if( pData[3] == 3 )//个数
					{
						if(pData[4] == Enum_698 )//方位
						{
							Buf[0] = pData[5]; 
							if( Buf[0] > 1 )//经度方位   enum{E（0），W（1）}，
							{
								return DAR_OverRegion;
							}	
							if( pData[6] == Long_unsigned_698 )//度
							{
								lib_ExchangeData( &Buf[1],&pData[7],2);
								if( pData[9] == Float32_698 )//分
								{
									lib_ExchangeData( &Buf[3],&pData[10],4);
									if( pData[14] == Structure_698 )//纬度结构体
									{
										if( pData[15] == 3 )//个数
										{
											if(pData[16] == Enum_698 )//方位
											{
												Buf[7] = pData[17]; 
												if( Buf[7] > 1 )//纬度方位   enum{S（0），N（1）}，
												{
													return DAR_OverRegion;
												}
												if( pData[18] == Long_unsigned_698 )//度
												{ 
													lib_ExchangeData( &Buf[8],&pData[19],2);
													if( pData[21] == Float32_698 )//秒
													{
														lib_ExchangeData( &Buf[10],&pData[22],4);
														if( pData[26] == Double_long_698)
														{	
															lib_ExchangeData(&Buf[14], &pData[27], 4);
															DAR = DAR_Success;
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	else if(( ClassIndex == 1) || ( ClassIndex == 2))
	{
		OffestLen = (ClassIndex == 2)? 7:0;//确定偏移地址
		
		if( pData[0] == Structure_698 )//经度结构体
		{
			if( pData[1] == 3 )//个数
			{
				if(pData[2] == Enum_698 )//方位
				{
					Buf[OffestLen] = pData[3]; 
					if( Buf[OffestLen] > 1 )//经度方位   enum{E（0），W（1）}，//纬度方位   enum{S（0），N（1）}，
					{
						return DAR_OverRegion;
					}	
					if( pData[4] == Long_unsigned_698 )//度
					{
						lib_ExchangeData( &Buf[OffestLen+1],&pData[5],2);
						if( pData[7] == Float32_698 )//分
						{
							lib_ExchangeData( &Buf[OffestLen+3],&pData[8],4);
							DAR = DAR_Success;
						}
					}
				}
			}
		} 
	}
	else if( ClassIndex == 3)
	{
		if( pData[0] == Double_long_698)
		{	
			lib_ExchangeData(&Buf[14], &pData[1], 4);
			DAR = DAR_Success;
		} 
	}

	if( DAR == DAR_Success)//代表数据正常解析
	{
 		Result = api_ProcMeterTypePara( WRITE, eMeterMeterPosition, Buf);
	 	if( Result == FALSE)
		{
			DAR = DAR_HardWare;//DAR 硬件失效
		}
		else
		{			
			DAR = DAR_Success;//DAR 成功
		}
	}

	return DAR;
}
//--------------------------------------------------
//功能描述:    设置时区时段表数
//         
//参数:      BYTE ClassAttribute[in]   元素属性
//         
//         BYTE ClassIndex[in]       属性索引
//         
//         BYTE *pData[in]           数据指针
//                  
//返回值:     BYTE DAR结果 
//         
//备注内容:    无
//--------------------------------------------------
BYTE Set_Timezone_TimeInterval_Num( BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )
{
	BYTE DAR,Buf[10],Length;
	WORD Result;
	
	Result = FALSE;
	DAR = DAR_WrongType;
	Length = 0;
	
	if( ClassAttribute != 0x02)//只能设置属性2
	{
		return DAR_Undefined;
	}

	if( ClassIndex == 0)//元素索引为零
	{
		if( pData[0] == Structure_698 )
		{
			if( pData[1] == 0x05 )
			{
				if( pData[2] == Unsigned_698 )//年时区数 不允许为0的设置
				{
					if((pData[3] <= MAX_TIME_AREA) && (pData[3] > 0) )
					{
						Buf[0] = pData[3];
						if( pData[4] == Unsigned_698 )//日时段表数 不允许为0的设置
						{
							if( (pData[5] <= MAX_TIME_SEG_TABLE) && (pData[5] > 0) )
							{
								Buf[1] = pData[5];
								if( pData[6]== Unsigned_698 )//日时段数 不允许为0的设置
								{
									if( (pData[7] <= MAX_TIME_SEG) && (pData[7] > 0) )
									{
										Buf[2] = pData[7];
										if( pData[8] == Unsigned_698 )//费率数
										{
											if( pData[9] <=  MAX_RATIO )
											{
												Buf[3] = pData[9];
												if( pData[10] == Unsigned_698 )//公共假日数
												{
													if( pData[11] <=  MAX_HOLIDAY )
													{
														Buf[4] = pData[11];
														Length = 5;
														DAR = DAR_Success;
														ClassIndex = 1;
													}
													else
													{
                                                        DAR = DAR_OverRegion;
													}
												}
											}
											else
											{
												DAR = DAR_OverRAte;
											}
										}
									}
									else
									{
										DAR = DAR_OverSlot;
									}
								}
							}
							else
							{
                                DAR = DAR_OverRegion;
							}
						}
					}
					else
					{
						DAR = DAR_OverZone;
					}
					
				}
			}
		}	
	}
	else//元素索引不为零
	{
		if( ClassIndex > 5)
		{
			return DAR;
		}

		pData = DealStructureClassIndexNotZeroData(pData);

		if( pData[0] == Structure_698 )
		{
			if( pData[1] == 0x01 )
			{
				if( pData[2] == Unsigned_698 )//年时区数
				{
				    Buf[0] = pData[3];
					Length = 1;
					
				    if( ClassIndex == 1 )//年时区数
				    {
                        if( (Buf[0] <= MAX_TIME_AREA) && (Buf[0] > 0) )
                        {
                            DAR = DAR_Success;
                        }
                        else
                        {
                            DAR = DAR_OverZone;
                        }
				    }
				    else if( ClassIndex == 2 )//日时段表数
				    {
                        if( (Buf[0] <= MAX_TIME_SEG_TABLE) && (Buf[0] > 0) )
                        {
                            DAR = DAR_Success;
                        }
                        else
                        {
                            DAR = DAR_OverRegion;
                        }
				    }
                    else if( ClassIndex == 3 )//日时段数
                    {
                        if( (Buf[0] <= MAX_TIME_SEG) && (Buf[0] > 0) )
                        {
                            DAR = DAR_Success;
                        }
                        else
                        {
                            DAR = DAR_OverSlot;
                        }
                    }
                    else if( ClassIndex == 4 )//费率数
                    {
                        if( Buf[0] <= MAX_RATIO )
                        {
                            DAR = DAR_Success;
                        }
                        else
                        {
                            DAR = DAR_OverRAte;
                        }
                    }
                    else//公共假日数
                    {
                        if( Buf[0] <= MAX_HOLIDAY )
                        {
                            DAR = DAR_Success;
                        }
                        else
                        {
                            DAR = DAR_OverRegion;
                        }
                    }

				}
			}
		}
	}
	
	if( DAR == DAR_Success)
	{
		Result = api_WritePara( 0, (GET_STRUCT_ADDR(TFPara1, TimeZoneSegPara.TimeZoneNum) +(ClassIndex-1)), Length, Buf);
		if( Result == FALSE )
		{
			DAR = DAR_HardWare; //此为固定返回，不可随意更改 需与上次进行配合 ---jwh
		}
		else
		{
			DAR = DAR_Success; 
		}
	}

	return DAR;
}

#if( PREPAY_MODE == PREPAY_LOCAL)
//--------------------------------------------------
//功能描述:    设置阶梯数
//         
//参数:      BYTE ClassAttribute[in]   元素属性
//         
//         BYTE ClassIndex[in]       属性索引
//         
//         BYTE *pData[in]           数据指针
//         
//返回值:     BYTE DAR结果 
//         
//备注内容:    无
//--------------------------------------------------
BYTE Set_LadderNum( BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )
{
	BYTE DAR;
	WORD Result;
	
	Result = FALSE;
	DAR = DAR_WrongType;
	
	if( ClassAttribute != 0x02)//只能设置属性2
	{
		return DAR_Undefined;
	}
	
    if( ClassIndex != 0)//元素索引为零
    {
        return DAR_Undefined;
    }

    if( pData[0] == Unsigned_698 )
    {
        Result = api_WritePrePayPara( eLadderNum, (BYTE*)&pData[1] );
		if( Result == FALSE )
		{
			DAR = DAR_HardWare; //此为固定返回，不可随意更改 需与上次进行配合 ---jwh
		}
		else
		{
			DAR = DAR_Success; 
		}
    }

	return DAR;
}
#endif

//--------------------------------------------------
//功能描述:    设置密钥总条数
//         
//参数:      BYTE ClassAttribute[in]   元素属性
//         
//         BYTE ClassIndex[in]       属性索引
//         
//         BYTE *pData[in]           数据指针
//         
//返回值:     BYTE DAR结果 
//         
//备注内容:    无
//--------------------------------------------------
BYTE SetEsamKeyTotalNum( BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )
{
	BYTE DAR,KeyTotalNum;
	WORD Result;
	
	Result = FALSE;
	DAR = DAR_WrongType;

	if( (ClassAttribute != 2) || ( ClassIndex != 0 ) )
	{
		return DAR_WrongType;
	}

	if( pData[0] == Unsigned_698 )
	{
		KeyTotalNum = pData[1];//密钥条数
		DAR = DAR_Success;
	}

	if( DAR == DAR_Success)
	{
		Result = api_WritePrePayPara(eKeyNum, (BYTE *)&KeyTotalNum);
		if( Result == FALSE )
		{
			DAR = DAR_HardWare; //此为固定返回，不可随意更改 需与上次进行配合 ---jwh
		}
		else
		{
			DAR = DAR_Success; 
		}
	}

	return DAR;
}
//--------------------------------------------------
//功能描述:    设置公共假日表
//         
//参数:      BYTE ClassAttribute[in]   元素属性
//         
//         BYTE ClassIndex[in]     属性索引
//         
//         BYTE *pData[in]           数据指针
//                         
//返回值:     BYTE DAR结果   
//         
//备注内容:    无
//--------------------------------------------------
BYTE SetTimeHolidayTable( BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )
{	
	BYTE DAR,Buf[10],i;
	WORD Result;
	TFourByteUnion OAD;
	TRealTimer t1;
	
	Result = FALSE;
	DAR = DAR_WrongType;
	memcpy(OAD.b,pData-4,4);//获取OAD

	if( ClassAttribute != 0x02)//只能设置属性2
	{
		return DAR_Undefined;
	}

	if( ClassIndex == 0 )//元素索引不是零的情况下 进行数据处理
	{
		return DAR_RefuseOp;
	}
	else//对应if( ClassIndex == 0 ):元素索引不是零的情况下 进行数据处理
	{	
		pData = DealArrayClassIndexNotZeroData(pData);
		if( ClassIndex > MAX_HOLIDAY )
		{
			return DAR_OverRegion;
		}
	}
	if( pData[0] == Array_698 )
	{
		if( pData[1] <= MAX_HOLIDAY)//判断最多公共假日数
		{
			for( i=0;i < pData[1];i++)
			{
				if( pData[2+i*10] == Structure_698 )
				{
					if( pData[3+i*10] == 02 )
					{
						if( pData[4+i*10] == Date_698 )	//取年月日周 5个字节
						{
							memset( (void *)&t1, 0x00, sizeof(t1) );
							memcpy( (void *)&(t1.wYear), (void *)&(pData[5+i*10]), 4);
							if( t1.wYear == 0xffff )
							{
								memcpy( Buf, &pData[5+i*10],5 );
							}
							else
							{		
								lib_InverseData((BYTE *)&t1.wYear, 2);
								if( api_CheckMonDay( t1.wYear, t1.Mon, t1.Day ) == FALSE )
								{
									return DAR_OverRegion;
								}
								if( pData[9+i*10] != ( ((api_CalcInTimeRelativeMin(&t1)/1440)+6)%7) )
								{
									return DAR_OverRegion;
								}
								memcpy( Buf, &t1.wYear, 4 );
								Buf[4]= pData[9+i*10];
							}

							/*
							if( api_CheckMonDay( (pData[5+i*10]*0x100+pData[6+i*10]), pData[7+i*10], pData[8+i*10] ) == FALSE )
							{
								return DAR_OverRegion;
							}
							lib_ExchangeData( Buf, &pData[5+i*10], 2 );
							memcpy( Buf+2, &pData[7+i*10],3 );
							*/
							if( pData[10+i*10] == Unsigned_698 )
							{
								Buf[5] = pData[11+i*10];

								// 写公共假日表之前先存之前的表
								if(OAD.b[3]==0)// 写全部的话拆分
								{
									OAD.b[3]=i+1;
									api_WritePreProgramData( 0,OAD.d );
									OAD.b[3]=0;
								}
								else
								{
									api_WritePreProgramData( 0,OAD.d );
								}
								
								if( ClassIndex == 0 )
								{
									Result = api_WriteTimeTable( 2, GET_STRUCT_ADDR(TTimeHolidayTable, TimeHoliday[i]), 6, Buf);
									if( Result == FALSE )//失败退出
									{
										DAR = DAR_HardWare;//此为固定返回，不可随意更改，与上层配合
										break;
									}
								}	
								else
								{
									if( pData[1] != 1)
									{
										break;
									}
									Result = api_WriteTimeTable( 2, GET_STRUCT_ADDR(TTimeHolidayTable, TimeHoliday[ClassIndex-1]), 6, Buf);
									if( Result == FALSE )//失败退出
									{
										DAR = DAR_HardWare;//此为固定返回，不可随意更改，与上层配合
										break;
									}
								}

								api_SavePrgOperationRecord( ePRG_HOLIDAY_NO );
							}
						}
						else
						{
							break;
						}
					}
				}

			}
		}
	}
	
	if( Result == TRUE)
	{
		DAR = DAR_Success;
		api_SetAllTaskFlag( eFLAG_SWITCH_JUDGE );
	}

	return DAR;
}


//--------------------------------------------------
//功能描述: 把时段参数多余的时区、时段用最后一个有效的补上
//         
//参数:      BYTE byLen[in]            设置数据长度
//         
//         BYTE byMaxLen[in]         最大数据长度
//
//         BYTE *pBuf[in]            数据指针
//         
//返回值:     BYTE DAR结果 
//         
//备注内容:    参数在全局变量 ProtocolBuf 中
//--------------------------------------------------
void api_AddTimeSegParaExtDataWithLastValid(BYTE byLen, BYTE byMaxLen, BYTE *pBuf )
{
	BYTE i;
	//如果读取的时区、时段数据达不到最大，用最后一个补上，因写的时候是用最大长度
	if( byLen < byMaxLen )
	{
		// 若设置时区、时段少于14个，应使用最后一个补齐
		for(i=0; i<(( byMaxLen - byLen)/3); i++)
		{
			memcpy(&pBuf[byLen + 3*i], &pBuf[byLen - 3], 3);
		}
	}
}
//--------------------------------------------------
//功能描述:    设置时区表
//         
//参数:      BYTE TYPE[in]             数据OI
//         
//         BYTE ClassAttribute[in]   元素属性
//
//         BYTE ClassIndex[in]       属性索引
//         
//         BYTE *pData[in]           数据指针
//                 
//返回值:     BYTE DAR结果 
//         
//备注内容:    无
//--------------------------------------------------
BYTE SetTimezoneTable( BYTE Type,BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )//时区
{
	BYTE DAR,i;
	WORD Result;
	TFourByteUnion OAD;
	
	Result = FALSE;
	DAR = DAR_WrongType;
	OAD.d = 0x00021540;
	
	if( (api_GetSysStatus( eSYS_STATUS_INSIDE_FACTORY ) == FALSE) && (Type == 0x01) )//非厂内模式下不允许设置时区表
	{
		return DAR_RefuseOp;//DAR_OtherErr;
	}
	
	if( ClassAttribute != 0x02)//只能设置属性2
	{
		return DAR_Undefined;
	}
	if( ClassIndex != 0)
	{
		pData = DealArrayClassIndexNotZeroData(pData);//元素索引不为零时对数据进行处理
	
		if( ClassIndex > MAX_TIME_AREA)//只能设置属性2
		{
			return DAR_OverRegion;
		}
	}

	if( pData[0] == Array_698 )
	{
		if( pData[1] > MAX_TIME_AREA)
		{
			pData[1] = MAX_TIME_AREA;
		}
		
		Result = DealTimeZoneSegTableData( &pData[1] );
	}
	
	if( Result == TRUE )//已经判断到最后一个时区，并且未出错
	{
		for( i=0; i<pData[1]; i++ )
		{
			if( api_CheckMonDay( RealTimer.wYear, ProtocolBuf[i*3], ProtocolBuf[i*3+1] ) == FALSE )
			{
				return DAR_OverRegion;
			}
		}
		
		api_WritePreProgramData( 0,OAD.d );
		if( ClassIndex == 0)
		{
			api_AddTimeSegParaExtDataWithLastValid(3*pData[1], (MAX_TIME_AREA*3), ProtocolBuf);
			Result = api_WriteTimeTable( Type, GET_STRUCT_ADDR(TTimeAreaTable, TimeArea[0][0]) ,(MAX_TIME_AREA*3), ProtocolBuf);
			if( Result == FALSE )
			{
				DAR = DAR_HardWare;//此为固定返回，不可随意更改，需要上层配合
			}
			
		}
		else
		{
			if( pData[1] != 1)
			{
				Result = FALSE;
			}
			else
			{
				Result = api_WriteTimeTable( Type, GET_STRUCT_ADDR(TTimeAreaTable, TimeArea[ClassIndex-1][0]) ,3, ProtocolBuf);
				if( Result == FALSE )
				{
					DAR = DAR_HardWare;//此为固定返回，不可随意更改，需要上层配合
				}
			}
		}
	}
	
	if( Result == TRUE)
	{
	    //api_WritePara( 0, (GET_STRUCT_ADDR(TFPara1, TimeZoneSegPara.TimeZoneNum)), 1, (BYTE*)&pData[1]);//设置时区数
		api_SavePrgOperationRecord( ePRG_TIME_AREA_NO ); //保存编程记录
		DAR = DAR_Success;
	}

	return DAR;
}
//--------------------------------------------------
//功能描述:    设置时段
//         
//参数:      BYTE TYPE[in]             数据OI
//         
//         BYTE ClassAttribute[in]   元素属性
//
//         BYTE ClassIndex[in]     属性索引
//         
//         BYTE *pData[in]           数据指针
//                  
//返回值:     BYTE DAR结果 
//         
//备注内容:    无
//--------------------------------------------------
BYTE SetTimeTable( BYTE Type,BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )//时段
{
	BYTE DAR,b;
	WORD Result;
	TFourByteUnion OAD;
	
	Result = FALSE;
	DAR = DAR_WrongType;
	memcpy(OAD.b,pData-4,4);//获取OAD	
	
	if( (api_GetSysStatus( eSYS_STATUS_INSIDE_FACTORY ) == FALSE) && (Type == 0x03) )//非厂内模式下不允许设置时段表
	{
		return DAR_OtherErr;
	}
	
	if( ClassAttribute != 0x02)//只能设置属性2
	{
		return DAR_Undefined;
	}
	//适应于面向对象数据交换协议的电能表及采集终端相关技术要求（20170412）中其它要求有"5)电能表时段表编程，一次只能设置一个时段表"
	if( ClassIndex == 0 )
	{
        return DAR_WrongType;
    }
    
	//pData = DealArrayClassIndexNotZeroData(pData);//元素索引不为零时对数据进行处理
	if( ClassIndex > MAX_TIME_SEG_TABLE)//只能设置属性2
	{
		return DAR_OverRegion;
	}		

	//api_WritePreProgramData( 0,OAD.d );
	
	if( pData[0] == Array_698 )
	{
		if( pData[1] > MAX_TIME_SEG )
		{
			pData[1] = MAX_TIME_SEG;
		}

		Result = DealTimeZoneSegTableData( &pData[1] );
		
		if( Result == TRUE )//对时段表间隔应小于15分钟进行判断
		{
			for( b=0; b<(3*pData[1]); b+=3 )
			{
				if( b <= (3*pData[1]-3) )//最后一个时段和第一个时段的间隔也要判断 2014-5-8
				{
					if(api_JudgeTimeSegInterval((BYTE*)&ProtocolBuf[b]) == FALSE )
					{
						Result = FALSE;
					}
				}

				if( ProtocolBuf[b] > 23 )//时
				{
					Result = FALSE;
				}
				else if( ProtocolBuf[b+1] > 59 )//分
				{
					Result = FALSE;
				}
				else if( 	( ProtocolBuf[b+2] == 0x00 )
						||	( ProtocolBuf[b+2] > MAX_RATIO) )
				{
					Result = FALSE;
				}
			}
		}
		if( Result == TRUE )// 1套时段表解析成功
		{	
		    //api_WritePara( 0, (GET_STRUCT_ADDR(TFPara1, TimeZoneSegPara.TimeSegNum)), 1, (BYTE*)&pData[1]);//根据设置设置时段表数
			// 写时段表之前先存之前的表
			api_WritePreProgramData( 0,OAD.d );

			api_AddTimeSegParaExtDataWithLastValid(3*pData[1], (MAX_TIME_SEG*3), ProtocolBuf);
			Result = api_WriteTimeTable( (Type+ClassIndex-1), GET_STRUCT_ADDR(TTimeSegTable, TimeSeg[0][0]),(MAX_TIME_SEG*3), ProtocolBuf);
			if( Result == FALSE )
			{
				DAR = DAR_HardWare;//此为固定返回，不可随意更改，需要上层配合
			}
			else
			{
				api_SavePrgOperationRecord( ePRG_TIME_TABLE_NO ); // 对于拆分的时段表编程，每个时段表保存一条记录
			}
			
		}
		else
		{
			DAR = DAR_WrongType;//此为固定返回，不可随意更改，需要上层配合
		}

	}	
	
	if( Result == TRUE)
	{
		DAR = DAR_Success;
		//api_SavePrgOperationRecord(ePRG_TIME_TABLE_NO);
	}

	return DAR;

}
//--------------------------------------------------
//功能描述:    设置结算日
//         
//参数:      BYTE ClassAttribute[in]   元素属性
//
//         BYTE ClassIndex[in]       属性索引
//         
//         BYTE *pData[in]           数据指针
//                  
//返回值:     BYTE DAR结果 
//         
//备注内容:    无
//--------------------------------------------------
BYTE SetBillingPara( BYTE ClassAttribute,BYTE ClassIndexes,BYTE *pData )//暂未考虑ClassIndex不为零的情况
{
	BYTE byNeedSwitchSave;
	BYTE DAR,i;	
	WORD Result;
	TBillingPara TmpBillingPara;
	TFourByteUnion OAD;
	
	//memcpy(OAD.b,pData-4,4);//获取OAD
	OAD.d = 0x00021641;

	Result = FALSE;
	DAR = DAR_WrongType;
	Result = api_VReadSafeMem( GET_SAFE_SPACE_ADDR( ParaSafeRom.BillingPara ),sizeof(TBillingPara), (BYTE *)&TmpBillingPara);

	if( ClassIndexes != 0 )
	{
		pData = DealArrayClassIndexNotZeroData(pData);//元素索引不为零时对数据进行处理
		if( ClassIndexes > MAX_MON_CLOSING_NUM )
		{
			return DAR_OverRegion;
		}
		ClassIndexes = ClassIndexes-1;
	}
	byNeedSwitchSave = 0;
	if( ClassAttribute == 2 )
	{
		if( pData[0] == Array_698 )
		{
			if( pData[1] <= MAX_MON_CLOSING_NUM )
			{
				for( i=0; i <pData[1]; i++ )
				{
					if( pData[2+6*i] == Structure_698 )
					{
						if(( pData[4+6*i] == Unsigned_698 ) && (pData[6+6*i] == Unsigned_698))
						{
							//判断结算日，日时应放在一起判断
							if(((pData[5+6*i] > 0) && (pData[5+6*i] <= 28) && (pData[7+6*i] <= 23) )//日范围为1到28,小时为0到23
							||((pData[5+6*i] == 0x99) && (pData[7+6*i] == 0x99))//设置日时均为0x99,此结算日为无效允许设置
							||((pData[5+6*i] == 0xFF) && (pData[7+6*i] == 0xFF))//设置日时均为0xff,此结算日为无效允许设置
							||((pData[5+6*i] == 99) && (pData[7+6*i] == 99)))//设置日时均为99,此结算日为无效允许设置
							{
								if( (ClassIndexes == 0) && (i == 0) )
								{
									if( (TmpBillingPara.MonSavePara[0].Hour != pData[7+6*i])||(TmpBillingPara.MonSavePara[0].Day != pData[5+6*i]) )
									{
										byNeedSwitchSave = 1;//如果改变第一结算日，则要转存月度用电量、结算日电量
									}
								}	
								TmpBillingPara.MonSavePara[i+ClassIndexes].Day = pData[5+6*i];
								TmpBillingPara.MonSavePara[i+ClassIndexes].Hour = pData[7+6*i];
							}
							else
							{
								DAR = DAR_OverRegion;//越界
								break;//跳出循环
							}

						}
						else
						{
                            break;
						}
					}
					else
					{
                        break;
					}
				}

				if( i == pData[1] )
				{
					DAR = DAR_Success;
				}
			}
		}
	}

	if( DAR == DAR_Success )
	{	
		api_WritePreProgramData( 0,OAD.d );
		Result = api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( ParaSafeRom.BillingPara ),sizeof(TBillingPara), (BYTE *)&TmpBillingPara);
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;
		}
		else
		{
			DAR = DAR_Success;			
			if( byNeedSwitchSave != 0 )//如果改变第一结算日，则要转存月度用电量、结算日电量 
			{
				api_SetFreezeFlag( eFREEZE_CLOSING, 0 ); //结算冻结
			}
			api_SavePrgOperationRecord( ePRG_CLOSING_DAY_NO );
		}
	}

    return DAR;
}
//--------------------------------------------------
//功能描述:    设置通讯地址、表号、客户编号参数
//         
//参数:      WORD OI[in]               数据OI
//         
//         BYTE ClassAttribute[in]   元素属性
//
//         BYTE ClassIndex[in]       属性索引
//         
//         BYTE *pData[in]           数据指针  
//                           
//返回值:     BYTE DAR结果   
//         
//备注内容:    无
//--------------------------------------------------
BYTE SetCommAddrOrMeterNoOrCustomCode( WORD OI,BYTE ClassAttribute,BYTE ClassIndexes,BYTE *pData ,BYTE Ch)//暂未考虑ClassIndexes不为零的情况
{
	WORD Result;
	BYTE Buf[10],DAR;

	DAR = DAR_WrongType;
	Result = FALSE;
	
	if( (ClassAttribute != 0x02) ||( ClassIndexes != 0 ))//只能设置属性2
	{
		return DAR_Undefined;
	}
	if( (pData[0] != Octet_string_698) || (pData[1] != 6) )
	{
		return DAR_Undefined;
	}
	memset(Buf,0x00,6);
	memcpy( Buf,&pData[2],6);
	if( OI == 0x4001)//通讯地址
	{
		Result =api_ProcMeterTypePara( WRITE, eMeterCommAddr, Buf);
		lib_ExchangeData( LinkData[Ch].ServerAddr, Buf, 6 );
	}
	else if( OI == 0x4002 )//表号
	{
	    if( api_GetSysStatus( eSYS_STATUS_INSIDE_FACTORY ) == TRUE )//厂内模式下 允许设置表号
	    {
           Result =api_ProcMeterTypePara( WRITE, eMeterMeterNo, Buf);
	    }	
	}
	else
	{
		if( api_GetSysStatus( eSYS_STATUS_INSIDE_FACTORY ) == TRUE )//厂内模式下 允许设置表号
	    {
		    Result = api_ProcMeterTypePara( WRITE, eMeterCustomCode, Buf );
		}
	}
	if( Result == FALSE)
	{
		DAR = DAR_HardWare;//此为固定错误返回，不可随意更改，与上层调用函数有关
	}
	else
	{			
		DAR = DAR_Success;//DAR 成功
	}

	return DAR;
}
//--------------------------------------------------
//功能描述:    设置时间
//         
//参数:      BYTE ClassAttribute[in]   元素属性
//
//         BYTE ClassIndex[in]       属性索引
//         
//         BYTE *pData[in]           数据指针
//                  
//返回值:     BYTE DAR结果    
//         
//备注内容:    无
//--------------------------------------------------
BYTE SetRTC( BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )//暂未考虑ClassIndex不为零的情况
{
	BYTE DAR;
	WORD Result;
	TRealTimer TmpRealTimer;//时间结构体
	TFourByteUnion OAD;
	TTimeBroadCastPara tmpTimeBroadCastPara;
	
	DAR = DAR_WrongType;
	memcpy(OAD.b,pData-4,4);//获取OAD

	if( ClassAttribute == 0x02 )//设置时间
	{
		if( ClassIndex != 0 )//只能设置属性2
		{
			return DAR_Undefined;
		}
			
		if( pData[0] != DateTime_S_698 )
		{
			return DAR_Undefined;
		}
		
		api_WritePreProgramData( 0,OAD.d );
		lib_InverseData(&pData[1], 2);
		memcpy((BYTE*)&(TmpRealTimer.wYear),&pData[1],sizeof( TRealTimer ));
		Result = api_WriteMeterTime(&TmpRealTimer);
		
		if( Result == FALSE )
		{
			DAR= DAR_HardWare;//此为固定错误返回，不可随意更改，与上层调用函数有关
		}
		else
		{	
			api_SetAllTaskFlag( eFLAG_TIME_CHANGE );
			api_SavePrgOperationRecord( ePRG_ADJUST_TIME_NO );
			#if( SEL_DEMAND_2022 == NO )//功率需量计算方式
			#if( MAX_PHASE != 1)		
			api_InitDemand( );//设置成功后，重新进行需量的计算
			#endif
			#endif
			DAR= DAR_Success;//DAR 成功
		}
	}
	else if( ClassAttribute == 0x05 )//属性5 广播校时参数
	{
		if( ClassIndex > 2 )
		{
			return DAR_RefuseOp;
		}
		
		//读取广播校时参数
		api_VReadSafeMem( GET_SAFE_SPACE_ADDR( ProSafeRom.TimeBroadCastPara), sizeof(TTimeBroadCastPara), (BYTE*)&tmpTimeBroadCastPara );

		if( ClassIndex == 0 )
		{
			if( pData[0] == Structure_698 )
			{
				if( pData[1] == 2 )
				{
					if( pData[2] == Long_unsigned_698 )//广播校时最小响应限值
					{
						lib_ExchangeData( (BYTE*)&tmpTimeBroadCastPara.TimeMinLimit, pData+3, 2 );	
						if( pData[5] == Long_unsigned_698 )//广播校时最大响应限值
						{
							lib_ExchangeData( (BYTE*)&tmpTimeBroadCastPara.TimeMaxLimit, pData+6, 2 );
							if( tmpTimeBroadCastPara.TimeMaxLimit > tmpTimeBroadCastPara.TimeMinLimit )//协议要求最大值大于最小值
							{
								DAR= DAR_Success;//DAR 成功
								api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( ProSafeRom.TimeBroadCastPara), sizeof(TTimeBroadCastPara), (BYTE*)&tmpTimeBroadCastPara );
							}
							else
							{
								DAR= DAR_OverRegion;//DAR 越界
							}
							
						}
					}
				}
			}
		}
		else
		{
			if( pData[0] == Long_unsigned_698 )
			{
				lib_ExchangeData( (BYTE*)&tmpTimeBroadCastPara.TimeMinLimit+(ClassIndex-1)*2, pData+1, 2 );
				if( tmpTimeBroadCastPara.TimeMaxLimit > tmpTimeBroadCastPara.TimeMinLimit )//协议要求最大值大于最小值
				{
					DAR= DAR_Success;//DAR 成功
					api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( ProSafeRom.TimeBroadCastPara), sizeof(TTimeBroadCastPara), (BYTE*)&tmpTimeBroadCastPara );	
				}
				else
				{
					DAR= DAR_OverRegion;//DAR 越界
				}

			}
		}
		
	}
	else
	{
		DAR = DAR_RefuseOp;
	}
	


    return DAR;
}

//--------------------------------------------------
//功能描述: 	切换时间格式判断
//       
//参数:     	Mode[in] 设置模式0x00：正常模式         0x55：esam数据更新
//			BYTE *pData[in] 时间数据
//			TmpRealTimer[out] 输出时间
//         
//返回值:     	BYTE DAR结果      
//         
//备注内容:    	无
//--------------------------------------------------
BYTE JudgeSwitchTimeformat( BYTE Mode, BYTE *pData, TRealTimer *RealTimer )
{
	BYTE Len;
	WORD Year;
	TRealTimer tmpRealTimer;

	memset( &tmpRealTimer, 0x00, sizeof(TRealTimer) );
	Year = 0;
	
	if( Mode == 0x00 )
	{
		Len = 6;
	}
	else
	{
		Len = 5;
	}
	
	if( lib_IsAllAssignNum(pData, 0x00, Len) == TRUE )
    {
    	memset( (BYTE*)&tmpRealTimer.wYear, 0x00, 6);
    }
    else if( lib_IsAllAssignNum(pData, 0x99, Len) == TRUE )
    {
    	memset( (BYTE*)&tmpRealTimer.wYear, 0x99, 6);
    }
    else if( lib_IsAllAssignNum(pData, 0xff, Len) == TRUE )
    {
    	memset( (BYTE*)&tmpRealTimer.wYear, 0xff, 6);
    }
    else
    {
		if( Mode == 0x55 )//esam数据更新
		{
			Year = 2000;		
			lib_MultipleBBCDToBin( pData, pData, 5 );
			Year = Year+pData[0];//获取年时间
			tmpRealTimer.wYear = Year;
			memcpy( &tmpRealTimer.Mon, pData+1, 4 );
		}
		else
		{
			lib_InverseData(pData, 2);
			memcpy( (BYTE*)&tmpRealTimer.wYear, pData, 6);//将时间转化为结构体函数
		}

		if( api_CheckMonDay( tmpRealTimer.wYear, tmpRealTimer.Mon, tmpRealTimer.Day ) == FALSE )
		{
			return DAR_OverRegion;
		}
		
		if( tmpRealTimer.Hour > 23 )
		{
			return DAR_OverRegion;
		}
		if( tmpRealTimer.Min > 59 )	
		{
			return DAR_OverRegion;
		}
    }
    
    memcpy( RealTimer, &tmpRealTimer, sizeof(TRealTimer) );
    
	return DAR_Success;
}


//--------------------------------------------------
//功能描述:    设置时区时段表的切换时间
//       
//参数:      BYTE Mode[in]             设置模式0x00：正常模式         0x55：esam数据更新
//         
//         WORD OI[in]               数据OI
//
//         BYTE ClassAttribute[in]   元素属性
//         
//         BYTE ClassIndexes[in]       属性索引
//         
//         BYTE *pData[in]           数据指针
//         
//返回值:     BYTE DAR结果      
//         
//备注内容:    无
//--------------------------------------------------
BYTE SetSwitchTime( BYTE Mode, WORD OI,BYTE ClassAttribute,BYTE ClassIndexes,BYTE *pData )//暂未考虑ClassIndex不为零的情况
{
	BYTE DAR,Type;		
	WORD Result;
	TRealTimer TmpRealTimer;//时间结构体
	DWORD RelativeTime;
		
	Type = OI - 0x4008;//获取偏移
	DAR = DAR_WrongType;
		
	if( (ClassAttribute != 0x02) || ( ClassIndexes != 0 ))//只能设置属性2 且不支持属性不为0的设置
	{
		return DAR_Undefined;
	}

	if( Mode == 0x00 )
	{

    	if( pData[0] != DateTime_S_698 )//必须是DateTime_S
    	{
    		return DAR_Undefined;
    	}
    	
		DAR = JudgeSwitchTimeformat( Mode, pData+1, &TmpRealTimer );
		if( DAR != DAR_Success )
		{
			return DAR;
		}
		
	}
	else
	{
	    if( pData[0] != 5 )
	    {
            return DAR_RefuseOp;
	    }
	    else
	    {
			DAR = JudgeSwitchTimeformat( Mode, pData+1, &TmpRealTimer );
			if( DAR != DAR_Success )
			{
				return DAR;
			}
	    }

	    //验证esam
		Result = api_AuthDataSID( ProtocolBuf );
		if( Result == FALSE )
		{
			return DAR_EsamFial;
		}
	}
	
	Result = api_ReadAndWriteSwitchTime( WRITE, Type, (BYTE *)&TmpRealTimer );

	if( Result == FALSE )
	{
		DAR = DAR_OverRegion;//此为固定错误返回，不可随意更改，与上层调用函数有关
	}
	else
	{
		DAR = DAR_Success;
	}

    return DAR;
}
#if( MAX_PHASE != 1 )	
//--------------------------------------------------
//功能描述:    设置需量参数
//         
//参数:      WORD OI[in]               数据OI
//         
//         BYTE ClassAttribute[in]   元素属性
//
//         BYTE ClassIndex[in]     属性索引
//         
//         BYTE *pData[in]           数据指针
//                  
//返回值:     BYTE DAR结果      
//         
//备注内容:    无
//--------------------------------------------------
BYTE SetDemandPara( WORD OI,BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )
{
	BYTE Buf[10],Type,DAR;
	WORD Result;
	
	DAR = DAR_WrongType;
	if( (ClassAttribute != 0x02) || ( ClassIndex != 0 ))//只能设置属性2 且不支持属性不为0的设置
	{
		return DAR_Undefined;
	}
	if( pData[0] == Unsigned_698 )
	{
		Buf[0] = pData[1];
	}
	else
	{
		return DAR_Undefined;
	}

	Type  = (OI - 0x4100);
	Result = api_WritePara( 1,  GET_STRUCT_ADDR(TFPara2, EnereyDemandMode.DemandPeriod)+Type, 1, Buf);
	
	if( Result == FALSE )
	{
		DAR = DAR_HardWare;//此为固定错误返回，不可随意更改，与上层调用函数有关
	}
	else
	{
		DAR = DAR_Success;
		#if( MAX_PHASE != 1)//功率需量-电能需量-更改需量参数后都重新进行计算需量		
		api_InitDemand( );//设置成功后，重新进行需量的计算
		#endif		
	}

    return DAR;
}
#endif//( MAX_PHASE != 1 )	
//--------------------------------------------------
//功能描述:    设置周休日特征字
//         
//参数:      BYTE ClassAttribute[in]   元素属性
//
//         BYTE ClassIndex[in]     属性索引
//         
//         BYTE *pData[in]           数据指针
//                
//返回值:     BYTE DAR结果   
//         
//备注内容:    无
//--------------------------------------------------
BYTE SetWeekStatus( BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )
{
	BYTE Buf[10],DAR;
	WORD Result;
	TFourByteUnion OAD;
	
	memcpy(OAD.b,pData-4,4);//获取OAD
		
	DAR = DAR_WrongType;
	if( (ClassAttribute != 0x02) || ( ClassIndex != 0 ))//只能设置属性2 且不支持属性不为0的设置
	{
		return DAR_Undefined;
	}
	if( pData[0] == Bit_string_698 )
	{
		//Buf[0] = pData[2] ;
		lib_ExchangeBit( (BYTE*)&Buf[0], (BYTE*)&pData[2], 1);
		DAR = DAR_Success;
	}
	else
	{
		return DAR_Undefined;
	}
	
	if( DAR == DAR_Success)
	{
		api_WritePreProgramData( 0,OAD.d );	
		Result = api_WritePara(0, GET_STRUCT_ADDR(TFPara1, TimeZoneSegPara.WeekStatus), 1, Buf);
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;//此为固定返回，不可随意更改，与上层配合
		}
		else
		{			
			DAR = DAR_Success;//DAR 成功
			api_SetAllTaskFlag( eFLAG_SWITCH_JUDGE );
			api_SavePrgOperationRecord( ePRG_WEEKEND_NO );
		}	
	}

	return DAR;
}
//--------------------------------------------------
//功能描述:    设置周休日时段
//         
//参数:      BYTE ClassAttribute[in]   元素属性
//
//         BYTE ClassIndex[in]     属性索引
//         
//         BYTE *pData[in]           数据指针
//                  
//返回值:     BYTE DAR结果     
//         
//备注内容:    无
//--------------------------------------------------
BYTE SetWeekSeg( BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )
{
	BYTE Buf[10],DAR;
	WORD Result;
	TFourByteUnion OAD;

	DAR = DAR_WrongType;
	memcpy(OAD.b,pData-4,4);//获取OAD
	
	if( (ClassAttribute != 0x02) || ( ClassIndex != 0 ))//只能设置属性2 且不支持属性不为0的设置
	{
		return DAR_Undefined;
	}

	if( pData[0] == Unsigned_698 )
	{
		Buf[0] = pData[1] ;
		DAR = DAR_Success;
	}
	else
	{
		return DAR_Undefined;
	}

	if( DAR == DAR_Success)
	{
		api_WritePreProgramData( 0,OAD.d );
		Result = api_WritePara(0, GET_STRUCT_ADDR(TFPara1, TimeZoneSegPara.WeekSeg), 1, Buf);
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;//DAR 硬件失效
		}
		else
		{			
			DAR = DAR_Success;//DAR 成功
			api_SetAllTaskFlag( eFLAG_SWITCH_JUDGE );
			api_SavePrgOperationRecord( ePRG_WEEKEND_NO );
		}
	}

	return DAR;
}
//--------------------------------------------------
//功能描述:    设置PTCT
//         
//参数:      WORD OI[in]               数据OI
//
//         BYTE ClassAttribute[in]   元素属性
//         
//         BYTE ClassIndex[in]       属性索引
//         
//         BYTE *pData[in]           数据指针
//         
//         
//返回值:     BYTE DAR结果  
//         
//备注内容:    无
//--------------------------------------------------
BYTE SetPTCT(     WORD OI,BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )
{
	BYTE DAR;
	WORD Result;
	DWORD PTCT;
	
	DAR = DAR_WrongType;
    PTCT = 0;
    
	if( (ClassAttribute != 0x02) || ( ClassIndex != 0 ))//只能设置属性2 且不支持属性不为0的设置
	{
		return DAR_Undefined;
	}
	
	if( pData[0] == 3 )
	{
	    lib_ExchangeData( (BYTE*)&PTCT, pData+1, 3 );
	    PTCT = lib_DWBCDToBin( PTCT );
		DAR = DAR_Success;
	}
	else
	{
		return DAR_Undefined;
	}
	
    //验证esam
	Result = api_AuthDataSID( ProtocolBuf );
	if( Result == FALSE )
	{
		return DAR_EsamFial;
	}
	
	if( DAR == DAR_Success)
	{
		if( OI == 0x401c)
		{
			Result = api_WritePrePayPara(eCTCoe, (BYTE*)&PTCT);
		}
		else
		{
			Result = api_WritePrePayPara(ePTCoe, (BYTE*)&PTCT);
		}
		
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;//此为固定返回，不可随意更改，请于上层进行配合
		}
		else
		{			
			DAR = DAR_Success;//DAR 成功
		}
	}
	
	return DAR;
}
#if( PREPAY_MODE == PREPAY_LOCAL)

//--------------------------------------------------
//功能描述:    设置报警金额限值
//         
//参数:      BYTE Ch[in]               通道选择
//         
//         WORD OI[in]               数据OI
//
//         BYTE ClassAttribute[in]   元素属性
//         
//         BYTE ClassIndex[in]     属性索引
//         
//         BYTE *pData[in]           数据指针
//         
//返回值:     BYTE DAR结果  
//         
//备注内容:    无
//--------------------------------------------------
BYTE SetLimitMoney( BYTE Mode, WORD OI,BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )
{
	BYTE DAR,Num,Num1,i,Type;
	WORD Result;
	DWORD LimitMoney1[4];
	DAR = DAR_WrongType;
	
	if( ClassAttribute != 0x02 )//只能设置属性2
	{
		return DAR_RefuseOp;
	}
	
    if( OI == 0x401e )
    {
        Num = 2;
        Type = eAlarm_Limit1;//报警金额1-WR
    }
    else if( OI == 0x401f )
    {
        Num = 3;
        Type = eTickLimit;//透支金额门限-WR
    }
    else
    {
		return DAR_Undefined;
    }

    if( Mode == 0 )//正常模式
    {
    	if( ClassIndex == 0 )
    	{
            if( pData[0] == Structure_698 )
            {
                if( pData[1] == Num )
                {
                    for( i = 0; i < Num; i++ )
                    {
                        if( pData[2+5*i] == Double_long_unsigned_698 )
                        {
                            lib_ExchangeData( (BYTE*)&LimitMoney1[i], &pData[3+5*i], 4);
                        }
                    }

                    if( i == Num )
                    {
                        DAR = DAR_Success;
                    }
                }
            }
    	}
    	else
    	{
            if( pData[0] == Double_long_unsigned_698 )
            {
                lib_ExchangeData( (BYTE*)&LimitMoney1[0], &pData[1], 4);
                DAR = DAR_Success;
            }
    	}

    }
    else//esam数据更新
    {
        if( OI != 0x401e )//esam数据更新只支持报警金额限值更新
        {
            return DAR_Undefined;
        }

        Num1 = ((ClassIndex==0) ? Num : 1);

        if( pData[0] != Num*4 )
        {
            return DAR_OverRegion;
        }
        
        for( i = 0; i < Num1; i++ )
        {
            //lib_MultipleBBCDToBin( (BYTE*)&pData[1+4*i], (BYTE*)&pData[1+4*i], 4 );
            lib_ExchangeData( (BYTE*)&LimitMoney1[i], (BYTE*)&pData[1+4*i], 4 );
            LimitMoney1[i] = lib_DWBCDToBin(LimitMoney1[i]);
        }

        if( i == Num1 )
        {
             DAR = DAR_Success;
        }

        //验证esam
		Result = api_AuthDataSID( ProtocolBuf );
		if( Result == FALSE )
		{
			return DAR_EsamFial;
		}
    }
	
	if( DAR == DAR_Success)
	{
	    Type = ((ClassIndex==0) ? Type : (Type+ClassIndex-1));
	    Num = ((ClassIndex==0) ? Num : 1);

	    for( i = 0; i < Num; i++ )
	    {
            Result = api_WritePrePayPara( (ePrePayParaType)(Type+i), (BYTE*)&LimitMoney1[i] );
            if( Result == FALSE )
            {
                DAR = DAR_RefuseOp;
                break;
            }
	    }
	}
	
	return DAR;
}

//--------------------------------------------------
//功能描述:    设置备用套费率电价
//         
//参数:      BYTE ClassAttribute[in]   元素属性
//         
//         BYTE ClassIndex[in]       属性索引
//         
//         BYTE *pData[in]           数据指针
//         
//返回值:     BYTE DAR结果  
//         
//备注内容:    无
//--------------------------------------------------
BYTE SetBackupRate( BYTE ClassAttribute,BYTE ClassIndex, BYTE *pData )
{
    BYTE RateNum,i,DAR;
    BOOL Result;
    TRatePrice RatePrice;
    TFourByteUnion Rate;

    DAR = DAR_Undefined;

    if( (ClassAttribute != 2) || (ClassIndex != 0))//只支持属性2的全部设置
    {
        return DAR_RefuseOp;
    }

    RateNum = (pData[0]/4);

    for( i=0; i<MAX_RATIO; i++ )
    {
        if( i < RateNum )
        {
            lib_ExchangeData( Rate.b, (BYTE*)&pData[1+4*i], 4 );//获取电价
            RatePrice.Price[i] = lib_DWBCDToBin(Rate.d);//BCD转化为Hex
        }
        else//对费率电价进行补位 补最后的费率
        {
            lib_ExchangeData( Rate.b, (BYTE*)&pData[1+4*(RateNum-1)], 4 );//获取电价
            RatePrice.Price[i] = lib_DWBCDToBin(Rate.d);//BCD转化为Hex
        }
    }
    
    //验证esam
	Result = api_AuthDataSID( ProtocolBuf );
	if( Result == FALSE )
	{
		return DAR_EsamFial;
	}
	
    api_WritePreProgramData( 0,0x00221940 );//40192200——编程前备用套费率电价 
   

    Result = api_WritePrePayPara( eBackupRateTable, (BYTE*)RatePrice.Price );
    if( Result == TRUE )
    {
		api_SavePrgOperationRecord( ePRG_TARIFF_TABLE_NO ); //电能表费率参数表编程事件
        DAR = DAR_Success;
    }

    return DAR;
}

BYTE SetBackupLadder( BYTE ClassAttribute,BYTE ClassIndex, BYTE *pData )
{
    BYTE i,b,DAR;
    BOOL Result;
    TLadderPrice LadderPrice;
    TFourByteUnion Ladeer;

    DAR = DAR_Undefined;

    if( (ClassAttribute != 2) || (ClassIndex != 0))//只支持属性2的全部设置
    {
        return DAR_RefuseOp;
    }

    for( i=0; i<MAX_LADDER; i++ )//阶梯电量
    {
        if( i < MAX_ESAM_LADDER )
        {
            lib_ExchangeData( Ladeer.b, (BYTE*)&pData[1+4*i], 4 );//获取阶梯电量
            LadderPrice.Ladder_Dl[i] = lib_DWBCDToBin(Ladeer.d);//BCD转化为Hex
        }
        else//对费率电价进行补位 补最后的费率
        {
            lib_ExchangeData( Ladeer.b, (BYTE*)&pData[1+4*5], 4 );//获取阶梯电量
            LadderPrice.Ladder_Dl[i] = lib_DWBCDToBin(Ladeer.d);//BCD转化为Hex
        }
    }

    for( i=0; i<MAX_LADDER+1; i++ )//阶梯电价
    {
        if( i < MAX_ESAM_LADDER+1 )
        {
            lib_ExchangeData( Ladeer.b, (BYTE*)&pData[1+4*MAX_ESAM_LADDER+4*i], 4 );//获取阶梯电量
            LadderPrice.Price[i] = lib_DWBCDToBin(Ladeer.d);//BCD转化为Hex
        }
        else//对费率电价进行补位 补最后的费率
        {
            lib_ExchangeData( Ladeer.b, (BYTE*)&pData[1+4*5], 4 );//获取阶梯电量
            LadderPrice.Price[i] = lib_DWBCDToBin(Ladeer.d);//BCD转化为Hex
        }
    }

    memset( (BYTE*)LadderPrice.YearBill, 0x99, sizeof(LadderPrice.YearBill) );//先置结算日为无效

    for( i=0; i<MAX_YEAR_BILL; i++ )
    {
        if( i < MAX_ESAM_YEAR_BILL )
        {
            for( b=0; b<3; b++ )
            {
                LadderPrice.YearBill[i][b] = pData[1+4*MAX_ESAM_LADDER+4*(MAX_ESAM_LADDER+1)+3*i+b];
                if( lib_IsBCD(LadderPrice.YearBill[i][b]) == TRUE )
                {
                    LadderPrice.YearBill[i][b] = lib_BBCDToBin( LadderPrice.YearBill[i][b] );
                }
            }
        }
        else
        {
            memset( (BYTE*)&LadderPrice.YearBill[i][0], 99, 3);
        }

        if( (LadderPrice.YearBill[i][0] > 12) && (LadderPrice.YearBill[i][0] != 99) && (LadderPrice.YearBill[i][0] != 0xFF) )//Mon
        {
            return DAR_OverRegion;
        }

        if( (LadderPrice.YearBill[i][1] > 28) && (LadderPrice.YearBill[i][1] != 99) && (LadderPrice.YearBill[i][1] != 0xFF) )//Day
        {
            return DAR_OverRegion;
        }

        if( (LadderPrice.YearBill[i][2] > 23) && (LadderPrice.YearBill[i][2] != 99) && (LadderPrice.YearBill[i][2] != 0xFF) )//Hour
        {
            return DAR_OverRegion;
        }
    }
    
    //验证esam
	Result = api_AuthDataSID( ProtocolBuf );
	if( Result == FALSE )
	{
		return DAR_EsamFial;
	}
	
    api_WritePreProgramData( 0,0x00221B40 );//401B2200——编程前备用套阶梯电价 

    Result = api_WritePrePayPara( eBackupLadderTable, (BYTE*)&LadderPrice.Ladder_Dl );
    if( Result == TRUE )
    {
		api_SavePrgOperationRecord( ePRG_LADDER_TABLE_NO ); //电能表阶梯表编程事件
        DAR = DAR_Success;
    }

    return DAR;
}
#endif
//--------------------------------------------------
//功能描述:    设置资产管理编码
//         
//
//         BYTE ClassIndex[in]       属性索引
//         
//         BYTE *pData[in]           数据指针
//                  
//返回值:     BYTE DAR结果      
//            
//备注内容:    无
//--------------------------------------------------
BYTE SetPropertyCode( BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )//资产管理编码
{
	BYTE DAR;
	WORD Result;
	DAR = DAR_WrongType;
	
	if( (ClassAttribute != 0x02) || ( ClassIndex != 0 ))//只能设置属性2 且不支持属性不为0的设置
	{
		return DAR_Undefined;
	}
	
	if( (pData[0] == Visible_string_698) && (pData[1] == 32) )
	{
		DAR = DAR_Success;
	}
	else
	{
		return DAR_Undefined;
	}
	
	if( DAR == DAR_Success)
	{
		Result = api_ProcMeterTypePara( WRITE, eMeterPropertyCode, pData+2 );
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;//此为固定返回，不可随意更改，请于上层进行配合
		}
		else
		{			
			DAR = DAR_Success;//DAR 成功
		}	
	}
	
	return DAR;
}
//--------------------------------------------------
//功能描述:    设置组合特征字
//         
//参数:      WORD OI[in]               数据OI
//         
//         BYTE ClassAttribute[in]   元素属性
//
//         BYTE ClassIndex[in]     属性索引
//         
//         BYTE *pData[in]           数据指针
//                  
//返回值:     BYTE DAR结果  
//         
//备注内容:    无
//--------------------------------------------------
BYTE SetCOMStatus( WORD OI,BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )//组合模式字
{
	BYTE DAR,Buf[10],Type;
	WORD Result;
	TFourByteUnion OAD;

	DAR = DAR_WrongType;
	memcpy(OAD.b,pData-4,4);//获取OAD	
	
	if( (ClassAttribute != 0x02) || ( ClassIndex != 0 ))//只能设置属性2 且不支持属性不为0的设置
	{
		return DAR_Undefined;
	}
	
	if( pData[0] == Bit_string_698 )
	{
		if( pData[1] == 8 )
		{
			lib_ExchangeBit( (BYTE*)&Buf[0], (BYTE*)&pData[2], 1);
			DAR = DAR_Success;
		}
		else
		{
			return DAR_Undefined;
		}
	}
	else
	{	
		return DAR_Undefined;;
	}
	
	Type = (OI - 0x4112);
	
	if( DAR == DAR_Success )
	{	
		api_WritePreProgramData( 0,OAD.d );
		Result = api_WritePara(1, GET_STRUCT_ADDR(TFPara2, EnereyDemandMode.byActiveCalMode)+Type, 1, Buf);
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;//此为固定返回，不可随意更改，请于上层进行配合
		}
		else
		{
			DAR = DAR_Success;

			#if( PREPAY_MODE == PREPAY_LOCAL )
			//设置有功组合方式特征字，需要刷新一下阶梯值和阶梯电价
			api_SetUpdatePriceFlag( ePriceChangeEnergy );
			#endif
			
			if( Type == 0 )//有功组合编程
			{
				api_SavePrgOperationRecord( ePRG_P_COMBO_NO );
			}
			else
			{
				#if( SEL_PRG_INFO_Q_COMBO == YES )
				api_SavePrgOperationRecord( ePRG_Q_COMBO_NO );
				#endif
			}
		}
	}
	
	return DAR;
}
//--------------------------------------------------
//功能描述:    设置电压合格率参数
//         
//参数:      WORD OI[in]               数据OI
//         
//         BYTE ClassAttribute[in]   元素属性
//
//         BYTE ClassIndex[in]       属性索引
//         
//         BYTE *pData[in]           数据指针         
//         
//返回值:     BYTE DAR结果  
//         
//备注内容:    无
//--------------------------------------------------
BYTE SetVoltageRatePara( WORD OI,BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )//电压合格率参数
{
	BYTE DAR,Buf[10],i;
	WORD Result;
	
	DAR = DAR_WrongType;
	
	if( (ClassAttribute != 0x02) || ( ClassIndex > 4 ))//只能设置属性2 且不支持属性大于4的设置
	{
		return DAR_Undefined;
	}
	
	if( ClassIndex == 0 )
	{
		if( pData[0] == Structure_698 )
		{
			if( pData[1] == 4 )
			{
				for( i=0; i<4; i++ )
				{
					if( pData[2+3*i] == Long_unsigned_698 )
					{
						lib_ExchangeData(&Buf[2*i], &pData[2+3*i+1] , 2);
					}
					else
					{
						break;
					}
				}

				if( i == 4 )
				{
					DAR = DAR_Success;
				}
			}
		}
	}
	else
	{
		pData = DealStructureClassIndexNotZeroData(pData);

		if( pData[0] == Structure_698 )
		{
			if( pData[1] == 1 )
			{
				if( pData[2] == Long_unsigned_698 )
				{
					lib_ExchangeData( Buf, &pData[3] , 2);
					DAR = DAR_Success;
				}
			}
		}
	}

	if( DAR == DAR_Success )
	{
		Result = api_WriteEventPara( OI, ClassIndex, Buf );
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;//此为固定返回，不可随意更改，请于上层进行配合
		}
		else
		{
			DAR = DAR_Success;
		}
	}
	
	return DAR;
}
#if( MAX_PHASE != 1 )	
//--------------------------------------------------
//功能描述:    设置期间需量冻结周期
//         
//参数:      BYTE ClassAttribute[in]   元素属性
//
//         BYTE ClassIndex[in]       属性索引
//         
//         BYTE *pData[in]           数据指针
//                  
//返回值:     BYTE DAR结果   
//         
//备注内容:    无
//--------------------------------------------------
BYTE SetIntervalDemandFreezingPeriod( BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )//组合模式字
{
	BYTE DAR,Buf[10];
	WORD Result;
	
	DAR = DAR_WrongType;

	if( (ClassAttribute != 2) || ( ClassIndex != 0) )
	{
		return DAR_WrongType;
	}

	if( pData[0] == TI_698 )
	{
		Buf[0] = pData[1];
		lib_ExchangeData( &Buf[1], &pData[2], 2 );
		DAR = DAR_Success;
	}

	if( DAR == DAR_Success )
	{
		Result = api_WritePara(1, GET_STRUCT_ADDR(TFPara2, EnereyDemandMode.IntervalDemandFreezePeriod), sizeof( TIntervalDemandFreezePeriod ), Buf);
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;//此为固定返回，不可随意更改，请于上层进行配合
		}
		else
		{
			DAR = DAR_Success;
		}
	}

	return DAR;
}
#endif//#if( MAX_PHASE != 1 )	
//--------------------------------------------------
//功能描述:    设置电表参数
//         
//参数:      BYTE ClassIndex[in]     属性索引
//
//         BYTE *pData[in]         数据指针
//                  
//返回值:     BYTE DAR结果       
//         
//备注内容:    无
//--------------------------------------------------
BYTE SetMeterVersionInformation( BYTE ClassIndex,BYTE *pData )//设置电表版本信息
{
	BYTE DAR,i,Buf[40];
	WORD Result;
	WORD Len;

	Result = FALSE;
	DAR = DAR_WrongType;

	if( api_GetSysStatus( eSYS_STATUS_INSIDE_FACTORY ) == FALSE )//只支持厂内模式下的设置版本信息
	{
		return DAR_RefuseOp;
	}

	if( ClassIndex == 0 )
	{
		if( pData[0] == Structure_698 )
		{
			if( pData[1] == 6 )
			{
				if( pData[2] == Visible_string_698 )
				{
					if( pData[3] == 4 )
					{
						memcpy( Buf, pData+4, 4 );//厂商代码
						if( pData[8] == Visible_string_698 )
						{
							if( pData[9] == 4 )
							{
								memcpy( Buf+4, pData+10, 4 );//软件版本号
								if( pData[14] == Visible_string_698 )
								{
									if( pData[15] == 6 )
									{
										memcpy( Buf+8, pData+16, 6 );//软件版本日期
										if( pData[22] == Visible_string_698 )
										{
											if( pData[23] == 4 )
											{
												memcpy( Buf+14, pData+24, 4 );//硬件版 本号
												if( pData[28] == Visible_string_698 )
												{
													if( pData[29] == 6 )
													{
														memcpy( Buf+18, pData+30, 6 );//硬件版本日期
														DAR = DAR_Success;
													}
												}
											}											
										}
									}
								}
							}
						}						
					}
				}
			}
		}

		if( DAR == DAR_Success )
		{
			Len = 0;
			for( i=0; i < 5; i++ )
			{
				Result = api_ProcMeterTypePara(WRITE, eMeterFactoryCode+i, Buf+Len );
				if( Result == FALSE )
				{
					DAR = DAR_HardWare;//硬件错误
					break;
				}
				
				if( (i == 2) || (i == 4) )
				{
					Len += 6;
				}
				else
				{
					Len += 4;
				}
			}
		}
	}
	else
	{
		if( ClassIndex > 6 )
		{
			return DAR_OverRegion;//越界
		}

		pData = DealStructureClassIndexNotZeroData(pData);

		if( pData[2] == Visible_string_698 )
		{
			if( (ClassIndex == 2) || ( ClassIndex == 4 ) )
			{
				Len = 6;
			}
			else
			{
				Len = 4;
			}

			if( pData[3] == Len )
			{
				memcpy( Buf, pData+4, Len );
				DAR= DAR_Success;
			}

			if( DAR == DAR_Success )
			{
				if( ClassIndex < 6 )
				{
					Result = api_ProcMeterTypePara(WRITE, eMeterFactoryCode+ClassIndex-1, Buf+Len );
					if( Result == FALSE )
					{
						DAR = DAR_HardWare;//硬件错误
					}
				}
			}
		}
	}
	
	return DAR;
}

//--------------------------------------------------
//功能描述:    设置电表参数
//         
//参数:      BYTE ClassAttribute[in]   元素属性
//
//         BYTE ClassIndex[in]     属性索引
//         
//         BYTE *pData[in]           数据指针
//                 
//返回值:     BYTE DAR结果      
//         
//备注内容:    无
//--------------------------------------------------
BYTE SetMeter( BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )//电气设备
{
	BYTE DAR,i,Buf[40];
	WORD Result;

	Result = FALSE;
	DAR = DAR_WrongType;
	
	switch( ClassAttribute )
	{
		case 3://设置版本信息
			if(api_GetSysStatus( eSYS_STATUS_INSIDE_FACTORY ) == FALSE )//仅支持厂内模式下的设置
			{
				return DAR_RefuseOp;
			}
			DAR = SetMeterVersionInformation( ClassIndex, pData );
			break;
		case 4://生产日期
			if(api_GetSysStatus( eSYS_STATUS_INSIDE_FACTORY ) == FALSE )//仅支持厂内模式下的设置
			{
				return DAR_RefuseOp;
			}
			if( ClassIndex == 0 )
			{
				if( pData[0] == DateTime_S_698 )
				{
					if( api_CheckMonDay( (pData[1]*0x100+pData[2]), pData[3], pData[4] ) == FALSE )
					{
						return DAR_OverRegion;
					}
					if( (pData[5] > 23) && (pData[6] > 59) )
					{
						return DAR_OverRegion;
					}
					lib_ExchangeData( Buf, &pData[1], 2 );//年 进行倒序
					memcpy( &Buf[2],&pData[3],5); //月、日、时、分、秒
					DAR = DAR_Success;
					Result = api_ProcMeterTypePara(WRITE, eMeterProduceDate, Buf );
				}
			}
			break;
		case 7://允许跟随上报
			if( ClassIndex == 0 )
			{
				if( (pData[0] == Boolean_698) && (pData[1] <= 1) )
				{
					DAR = DAR_Success;
					Result = SetReportFlag(pData[1],eFollowReport);
				}
			}
			break;
			
		case 8://允许主动上报
			if( ClassIndex == 0 )
			{
				if( (pData[0] == Boolean_698) && (pData[1] <= 1) )
				{
					DAR = DAR_Success;
					Result = SetReportFlag(pData[1],eActiveReport);
				}
			}
			break;


		case 10://上报通道
			if( ClassIndex == 0 )
			{
				if( pData[0] == Array_698 )
				{
					if( pData[1] < MAX_COM_CHANNEL_NUM )
					{
						for( i=0; i < pData[1]; i++ )
						{
							if( pData[2+5*i] == OAD_698 )
							{
								memcpy( Buf+4*i,(BYTE*)&pData[2+5*i+1],4);
							}
							else
							{
								break;
							}
						}

						if( i == pData[1] )
						{
							Result = SetReportChannel( 0, pData[1], Buf);
							DAR = DAR_Success;
						}
					}
				}
			}
			else if( ClassIndex <= MAX_COM_CHANNEL_NUM )
			{
				if( pData[0] == OAD_698 )
				{
					memcpy( Buf,(BYTE*)&pData[1],4);
					Result = SetReportChannel( ClassIndex, 1 , Buf);
					DAR = DAR_Success;
				}
			}
			break;

		default:
			break;
	}

	if( DAR == DAR_Success )
	{
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;//此为固定返回，不可随意更改，请于上层进行配合
		}
		else
		{
			DAR = DAR_Success;
		}
	}

	return DAR;
}
//--------------------------------------------------
//功能描述:    设置一般密码
//         
//参数:      BYTE ClassAttribute[in]   元素属性
//
//         BYTE ClassIndex[in]     属性索引
//         
//         BYTE *pData[in]           数据指针
//                  
//返回值:     BYTE DAR结果      
//         
//备注内容:    无
//--------------------------------------------------

BYTE SetPassWord( BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )//设置密码
{
	BYTE DAR,Offest;
	WORD Result;
	TTwoByteUnion len;
	TMuchPassword TmpMuchPassword;
	
	DAR = DAR_WrongType;

	if( (ClassAttribute != 2) || ( ClassIndex != 0) )
	{
		return DAR_WrongType;
	}

	if( pData[0] == Visible_string_698 )
	{
	    Offest = Deal_698DataLenth( (BYTE*)&pData[1], len.b, ePROTOCOL_TYPE );
		if( len.w != 8 )//一般密码明文长度 小于7
        {
			return DAR_OverRegion;
        }  
        
		if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR(ParaSafeRom.MuchPassword), sizeof(TMuchPassword),(BYTE *)TmpMuchPassword.Password645) != TRUE )
		{
			return DAR_HardWare;
		}
		
		memcpy( TmpMuchPassword.Password698, &pData[Offest+1], MAX_698PASSWORD_LENGTH);
		DAR = DAR_Success;
	}

	if( DAR == DAR_Success )
	{
		//写入密码，自己计算校验
		Result = api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( ParaSafeRom.MuchPassword ), sizeof(TMuchPassword), (BYTE*)&TmpMuchPassword);
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;//此为固定返回，不可随意更改，请于上层进行配合
		}
		else
		{
			DAR = DAR_Success;
		}
	}

	return DAR;
}

//--------------------------------------------------
//功能描述:      设置信号强度
//         
//参数:        BYTE ClassAttribute[in]   元素属性
//
//           BYTE ClassIndexes[in]     属性索引
//         
//           BYTE *pData[in]           数据指针       
//         
//返回值:     BYTE DAR结果      
//         
//备注内容:    无
//--------------------------------------------------

BYTE SetGPRSdBm( BYTE ClassAttribute,BYTE ClassIndexes,BYTE *pData )//设置密码
{
	BYTE DAR,dBm;
	short Value;
	
	DAR = DAR_WrongType;

	if( (ClassAttribute != 9) || ( ClassIndexes != 0) )
	{
		return DAR_WrongType;
	}

	if( pData[0] == Long_698 )
	{
        lib_ExchangeData( (BYTE*)&Value, (BYTE*)&pData[1], 2 );

        if( Value >0 )
        {
            DAR = DAR_OverRegion;
        }
        else
        {
            DAR = DAR_Success;
            
            if( (labs(Value) > 107) || (Value == 0) )
            {
                dBm = 0;
            }
            else if( labs(Value) > 93 )
            {
                dBm = 1;
            }
            else if( labs(Value) > 71 )
            {
                dBm = 2;
            }
            else if( labs(Value) > 69 )
            {
                dBm = 3;
            }
            else
            {
                dBm = 4;
            }
            
            api_ProcLcdWirelessPara( WRITE, (BYTE*)&dBm);
        }
	}

	return DAR;
}

//--------------------------------------------------
//功能描述:    设置电表基本信息参数( 有功常数、无功常数等)  0x4104~410b
//         
//参数:      WORD OI[in]               数据OI
//
//         BYTE ClassAttribute[in]   元素属性
//         
//         BYTE ClassIndex[in]       属性索引
//         
//         BYTE *pData[in]           数据指针
//         
//返回值:     BYTE DAR结果    
//         
//备注内容:    无
//--------------------------------------------------
BYTE SetMeterBasicPara( WORD OI,BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )
{	
	BYTE DAR,Buf[35],Type;
	WORD Result;
	WORD len;

	Result = FALSE;
	DAR = DAR_WrongType;
	
	if(api_GetSysStatus( eSYS_STATUS_INSIDE_FACTORY ) == FALSE )//仅支持厂内模式下的设置
	{
		return DAR_RefuseOp;
	}
	if(ClassAttribute == 0x02)
	{
		if( (OI == 0x4109) ||(OI == 0x410A) )//电能表有无功常数
		{
			Type = (OI - 0x4109);
			if( pData[0] == Double_long_unsigned_698 )
			{
				DAR = DAR_Success;
				lib_ExchangeData(Buf, &pData[1], 4);
				Result = api_ProcMeterTypePara(WRITE, eMeterActiveConstant+Type, Buf );
			}
		}
		else
		{
			if( OI == 0x410B )//电能表型号是32个字节
			{
				len = 32;
				Type = eMeterMeterModel;
			}
			else if( (OI == 0x4107) ||(OI == 0x4108) )//有功无功准确等级是4个字节
			{
				len = 4;
				Type = OI- 0x4107 + eMeterPPrecision;
			}
			else if( OI == 0x4111 )//软件备案号
			{
				len = 16;
				Type = eMeterSoftRecord;
			}
			else//其他 6个字节
			{
				len = 6;
				Type = (OI - 0x4104) +eMeterRateVoltage;
			}
			if( pData[0] == Visible_string_698 )
			{
				if( pData[1] == len )
				{
					DAR = DAR_Success;
					memcpy(Buf, &pData[2], len);
					Result = api_ProcMeterTypePara(WRITE, Type, Buf );
				}
			}
		}
	}
	else
	{
		if (OI == 0x4106)//4106属性2为最大电流，属性3最小电流，属性4转折电流
			{
				if( pData[0] == Visible_string_698 )
				{
					if(pData[1] < 10)
					{
						if(ClassAttribute == 0x03)
						{
							Type = eMeterMinCurrent;
							DAR = DAR_Success;
							memcpy(Buf, &pData[1], pData[1]+1);
							Result = api_ProcMeterTypePara(WRITE, eMeterMinCurrent, Buf );
						}
						else if(ClassAttribute == 0x04)
						{
							DAR = DAR_Success;
							memcpy(Buf, &pData[1], pData[1]+1);
							Result = api_ProcMeterTypePara(WRITE, eMeterTurningCurrent, Buf );

						}
						else
						{
						}
					}

				}
			}
	}
	
	if( DAR == DAR_Success )
	{
		if( Result != FALSE )
		{
			DAR = DAR_Success;
		}
		else
		{
			DAR = DAR_HardWare;
		}
	}
	
	return DAR;
}

//--------------------------------------------------
//功能描述:    设置MCU内蓝牙参数F20B
//参数:      WORD OI[in]               数据OI
//         
//         BYTE ClassAttribute[in]   元素属性
//
//         BYTE ClassIndex[in]       属性索引
//         
//         BYTE *pData[in]           数据指针  
//                           
//返回值:     BYTE DAR结果   
//         
//备注内容:    无
//--------------------------------------------------
// static BYTE SetMcuF20B( BYTE Ch, BYTE ClassAttribute, BYTE ClassIndex, BYTE *pData )
// {
// 	BYTE i, iMax, Offset;
// 	TTwoByteUnion Len;
// 	BYTE TmpBuf[250] = {0};
// 	BYTE NumOfMaster, NumOfSlave;
// 	TSafeMem_dev_work dev_work;

// 	iMax = 0;
// 	i = 0;
// 	Len.w = 0;
// 	NumOfMaster = 0;
// 	NumOfSlave = 0;
	
// 	//F20B仅属性4从设备列表和属性6工作参数支持设置，属性5仅支持被BLE设置
// 	if( (ClassAttribute == 4) || (ClassAttribute == 7) )	//属性4 从设备列表，属性7 锁具列表
// 	{
// 		Offset = 0;
// 		if( ClassIndex == 0 )//设置全部从设备列表
// 		{
// 			if( pData[Offset] != Array_698 )
// 			{
// 				return DAR_WrongType;
// 			}
// 			Offset++;
// 			if( pData[Offset] != 3 )//从设备列表包含3个结构体
// 			{
// 				return DAR_OverRegion;
// 			}
// 			Offset++;
// 			iMax = 3;
// 			i = 0;
// 		}
// 		else
// 		{
// 			if( ClassIndex > 3 )//从设备个数最多3个
// 			{
// 				return DAR_OverRegion;
// 			}
// 			iMax = ClassIndex;
// 			i = ClassIndex - 1;
// 		}
		
// 		for( ; i < iMax; i++ )
// 		{
// 			if( pData[Offset] != Structure_698 )
// 			{
// 				return DAR_WrongType;
// 			}
// 			Offset ++;		
// 			if( pData[Offset] != 2 )//从设备结构体包含2个元素
// 			{
// 				return DAR_OverRegion;
// 			}
// 			Offset++;
// 			if( pData[Offset] != Octet_string_698 )
// 			{
// 				return 	DAR_WrongType;
// 			}
// 			Offset++;
// 			Deal_698DataLenth( (BYTE *)&pData[Offset], Len.b, ePROTOCOL_TYPE );
// 			if( Len.w != 6 )//从设备mac地址为6字节
// 			{
// 				return DAR_OverRegion;
// 			}
// 			Offset++;

// 			//memcpy( (BYTE *)&TmpBuf[0], (BYTE *)&pData[Offset], Len.w );//从设备蓝牙mac地址
// 			if (api_VReadSafeMem(GET_SAFE_SPACE_ADDR(SafeMem_dev_work),sizeof(TSafeMem_dev_work), (BYTE *)&dev_work) == FALSE)
// 			{
// 				return DAR_RefuseOp;
// 			}
// 			memcpy( (BYTE *)&dev_work.tDownMessage[i].DownMac, (BYTE *)&pData[Offset], Len.w );//从设备蓝牙mac地址
// 			Offset += Len.w;
// 			if( pData[Offset] != Octet_string_698 )
// 			{
// 				return 	DAR_WrongType;
// 			}
// 			Offset++;
// 			Deal_698DataLenth( (BYTE *)&pData[Offset], Len.b, ePROTOCOL_TYPE );
// 			//if( Len.w > MAX_SLAVE_BLE_PROPERTY_LEN )//从设备资产号最长MAX_SLAVE_BLE_PROPERTY_LEN字节
// 			//{
// 			//	return DAR_OverRegion;
// 			//}
// 			Offset++;

// 			//memcpy( (BYTE *)&TmpBuf[6+MAX_SLAVE_BLE_PROPERTY_LEN-Len.w], (BYTE *)&pData[Offset], Len.w );//从设备资产号
			
// 			//电表中保存的从设备列表按照最大长度保存，无数据的位置补0（TmpBuf初始化为全0）
// 			memset( (BYTE *)&TmpBuf[0], 0x00, sizeof(TmpBuf) );

// 			memcpy( (BYTE *)&TmpBuf[0], (BYTE *)&pData[Offset], sizeof(TmpBuf) );
// 			memcpy( (BYTE *)&dev_work.tDownMessage[i].DownID, (BYTE *)&TmpBuf[0], 16 );
// 			if (api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(SafeMem_dev_work),sizeof(TSafeMem_dev_work), (BYTE *)&dev_work) == FALSE)
// 			{
// 				return DAR_RefuseOp;
// 			}
// 			Offset += Len.w;
// 		}
// 	}
// 	else if( ClassAttribute == 5 )
// 	{
// 		//只有蓝牙通道，客户机地址为0xFE时，表示设置操作来自BLE，不是电表外的设备，允许设置连接信息，其余情况为只读

// 		Offset = 0;
// 		if( ClassIndex == 0 )
// 		{
// 			if( pData[Offset] != Array_698 )
// 			{
// 				return DAR_WrongType;
// 			}
// 			Offset++;
// 			if( pData[Offset] != 1 )//连接信息只有1个结构体，因为电表只有1个蓝牙
// 			{
// 				return DAR_OverRegion;
// 			}
// 			Offset++;
// 		}
// 		else
// 		{
// 			if( ClassIndex != 1 )//连接信息只有1个结构体，因为电表只有1个蓝牙
// 			{
// 				return DAR_OverRegion;
// 			}
// 		}

// 		if( pData[Offset] != Structure_698 )
// 		{
// 			return DAR_WrongType;
// 		}
// 		Offset++;
// 		if( pData[Offset] != 3 )//连接信息结构体包含3个元素
// 		{
// 			return DAR_OverRegion;
// 		}
// 		Offset++;
// 		if( pData[Offset] != OAD_698 )
// 		{
// 			return DAR_WrongType;
// 		}
// 		Offset++;
// 		memcpy( (BYTE *)&TmpBuf[0], (BYTE *)&pData[Offset], 4 );//端口号，注：读取电表的端口号时固定仍返回0xF20B0201
// 		Offset += 4;
		
// 		//连接的主设备
// 		if( pData[Offset] != Array_698 )
// 		{
// 			return DAR_WrongType;
// 		}
// 		Offset++;
// 		//if( pData[Offset] > MAX_MASTER_NUM )//最多连接2个主设备
// 		//{
// 		//	return DAR_OverRegion;
// 		//}
// 		NumOfMaster = pData[Offset];//若array长度为0，说明连接的主设备数量为0
// 		Offset++;
// 		for( i = 0; i < NumOfMaster; i++ )
// 		{
// 			if( pData[Offset] != Structure_698 )
// 			{
// 				return DAR_WrongType;
// 			}
// 			Offset++;
// 			if( pData[Offset] != 2 )
// 			{
// 				return DAR_OverRegion;
// 			}
// 			Offset++;
// 			if( pData[Offset] != Visible_string_698 )
// 			{
// 				return DAR_WrongType;
// 			}
// 			Offset++;
// 			Deal_698DataLenth( (BYTE *)&pData[Offset], Len.b, ePROTOCOL_TYPE );
// 			if( Len.w > 32 )//蓝牙描述符最长32字节
// 			{
// 				return DAR_OverRegion;
// 			}
// 			Offset++;
// 			memcpy( (BYTE *)&TmpBuf[4+i*38], (BYTE *)&pData[Offset], Len.w );//主设备蓝牙描述符
// 			Offset += Len.w;
// 			if( pData[Offset] != Octet_string_698 )
// 			{
// 				return DAR_WrongType;
// 			}
// 			Offset++;
// 			Deal_698DataLenth( (BYTE *)&pData[Offset], Len.b, ePROTOCOL_TYPE );
// 			if( Len.w != 6 )//mac地址固定6字节
// 			{
// 				return DAR_OverRegion;
// 			}
// 			Offset++;
// 			memcpy( (BYTE *)&TmpBuf[4+i*38+32], (BYTE *)&pData[Offset], Len.w );//主设备蓝牙MAC地址
// 			Offset += Len.w;
// 		}

// 		//连接的从设备
// 		if( pData[Offset] != Array_698 )
// 		{
// 			return DAR_WrongType;
// 		}
// 		Offset++;
// 		//if( pData[Offset] > MAX_SLAVE_NUM )//最多连接3个从设备
// 		//{
// 		//	return DAR_OverRegion;
// 		//}
// 		NumOfSlave = pData[Offset];//若array长度为0，说明连接的从设备数量为0
// 		Offset++;
// 		for( i = 0; i < NumOfSlave; i++ )
// 		{
// 			if( pData[Offset] != Structure_698 )
// 			{
// 				return DAR_WrongType;
// 			}
// 			Offset++;
// 			if( pData[Offset] != 2 )
// 			{
// 				return DAR_OverRegion;
// 			}
// 			Offset++;
// 			if( pData[Offset] != Visible_string_698 )
// 			{
// 				return DAR_WrongType;
// 			}
// 			Offset++;
// 			Deal_698DataLenth( (BYTE *)&pData[Offset], Len.b, ePROTOCOL_TYPE );
// 			if( Len.w > 32 )//蓝牙描述符最长32字节
// 			{
// 				return DAR_OverRegion;
// 			}
// 			Offset++;
// 			memcpy( (BYTE *)&TmpBuf[4+NumOfMaster*38+i*38], (BYTE *)&pData[Offset], Len.w );//从设备蓝牙描述符
// 			Offset += Len.w;
// 			if( pData[Offset] != Octet_string_698 )
// 			{
// 				return DAR_WrongType;
// 			}
// 			Offset++;
// 			Deal_698DataLenth( (BYTE *)&pData[Offset], Len.b, ePROTOCOL_TYPE );
// 			if( Len.w != 6 )//mac地址固定6字节
// 			{
// 				return DAR_OverRegion;
// 			}
// 			Offset++;
// 			memcpy( (BYTE *)&TmpBuf[4+NumOfMaster*38+i*38+32], (BYTE *)&pData[Offset], Len.w );//主设备蓝牙MAC地址
// 			Offset += Len.w;
// 		}
// 	}
// 	else if( ClassAttribute == 6 )
// 	{
// 		Offset = 0;
// 		if( ClassIndex == 0 )
// 		{
// 			if( pData[Offset] != Array_698 )
// 			{
// 				return DAR_WrongType;
// 			}
// 			Offset++;
// 			if( pData[Offset] != 1 )//工作参数只有1个结构体，因为电表只有1个蓝牙
// 			{
// 				return DAR_OverRegion;
// 			}
// 			Offset++;
// 		}
// 		else
// 		{
// 			if( ClassIndex != 1 )//工作参数只有1个结构体，因为电表只有1个蓝牙
// 			{
// 				return DAR_OverRegion;
// 			}
// 		}

// 		if( pData[Offset] != Structure_698 )
// 		{
// 			return DAR_WrongType;
// 		}
// 		Offset++;
// 		if( pData[Offset] != 4 )//工作参数结构体包含4个元素
// 		{
// 			return DAR_OverRegion;
// 		}
// 		Offset++;
// 		if( pData[Offset] != OAD_698 )
// 		{
// 			return DAR_WrongType;
// 		}
// 		Offset++;
// 		memcpy( (BYTE *)&TmpBuf[0], (BYTE *)&pData[Offset], 4 );//端口号，注：读取电表的端口号时固定仍返回0xF20B0201
// 		Offset += 4;
// 		if( pData[Offset] != Unsigned_698 )
// 		{
// 			return DAR_WrongType;
// 		}
// 		Offset++;
// 		memcpy( (BYTE *)&TmpBuf[4], (BYTE *)&pData[Offset], 1 );//发射功率档
// 		Offset++;
// 		if( pData[Offset] != Long_unsigned_698 )
// 		{
// 			return DAR_WrongType;
// 		}
// 		Offset++;
// 		memcpy( (BYTE *)&TmpBuf[5], (BYTE *)&pData[Offset], 2 );//广播间隔
// 		Offset += 2;
// 		if( pData[Offset] != Long_unsigned_698 )
// 		{
// 			return DAR_WrongType;
// 		}
// 		Offset++;
// 		memcpy( (BYTE *)&TmpBuf[7], (BYTE *)&pData[Offset], 2 );//扫描间隔
// 		Offset += 2;

// 		if (api_VReadSafeMem(GET_SAFE_SPACE_ADDR(SafeMem_dev_work),sizeof(TSafeMem_dev_work), (BYTE *)&dev_work) == FALSE)
// 		{
// 			return DAR_RefuseOp;
// 		}
// 		memcpy(&dev_work.work_para,&TmpBuf[4],sizeof(dev_work.work_para));
// 		if (api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(SafeMem_dev_work),sizeof(TSafeMem_dev_work), (BYTE *)&dev_work) == FALSE)
// 		{
// 			return DAR_RefuseOp;
// 		}
// 		api_delayinit_ble();
// 	}
// 	else
// 	{
// 		return DAR_RefuseOp;
// 	}

// 	return DAR_Success;
// }

//--------------------------------------------------
//功能描述:    设置输入输出设备参数
//         
//参数:      BYTE Ch[in]     通道选择
//         
//         BYTE *pOAD[in]  OAD数据指针
//         
//返回值:     eAPPBufResultChoice  返回添加Buf结果        
//         
//备注内容:
//--------------------------------------------------
eAPPBufResultChoice SetRequestInOuputPara( BYTE Ch, BYTE *pOAD )
{
	BYTE DAR,ClassAttribute,ClassIndex;
	BYTE *pData;
	TTwoByteUnion OI;
	eAPPBufResultChoice eResultChoice;
    
	pData = &pOAD[4];//获取数据指针
	lib_ExchangeData(OI.b,pOAD,2);//获取OI
	ClassAttribute = (pOAD[2]&0x0f);
	ClassIndex = pOAD[3];
	DAR = DAR_WrongType;
	
	switch(OI.w)
	{
		// case 0xF20B://蓝牙
	    //     DAR = SetMcuF20B( Ch, ClassAttribute,  ClassIndex, pData);
		// 	break;
		default:
			DAR = DAR_WrongType;
			break;		
	}
	
	eResultChoice = DLT698AddOneBuf(Ch,0x55, DAR);//是完整数据
	
	return eResultChoice;
}

//--------------------------------------------------
//功能描述:    设置安全模式参数
//         
//参数:      BYTE Ch[in]     通道选择
//         
//         BYTE *pOAD[in]  OAD数据指针
//         
//返回值:     eAPPBufResultChoice  返回添加Buf结果
//         
//备注内容:    无
//--------------------------------------------------
eAPPBufResultChoice SetRequestSafeMode( BYTE Ch, BYTE *pOAD )
{
    BYTE i;
	BYTE DAR,ClassAttribute,ClassIndex;
	BYTE *pData;
	TTwoByteUnion OI;
	eAPPBufResultChoice eResultChoice;
	
	pData = &pOAD[4];//获取数据指针
	lib_ExchangeData(OI.b,pOAD,2);//获取OI
	ClassAttribute = (pOAD[2]&0x1f);
	ClassIndex = pOAD[3];
	DAR = DAR_WrongType;		

	if( ClassAttribute == 3 )
	{
		if( ClassIndex != 0 )
		{
			if( ClassIndex > SafeModePara.Num )//设置其中一屏时只允许更改已设置的
			{
				 DAR = DAR_OverRegion;
			}
			else
			{
				if( pData[0] == Structure_698 )
				{
					if( pData[1] == 2 )
				    {
				    	if( pData[2] == OI_698 )
				       	{
				        	lib_ExchangeData((BYTE*)&SafeModePara.Sub[ClassIndex-1].OI, (BYTE*)&pData[3], 2);
				        	
				            if( pData[5] == Long_unsigned_698 )
				            {
				            	lib_ExchangeData((BYTE*)&SafeModePara.Sub[ClassIndex-1].wSafeMode, (BYTE*)&pData[6], 2);
				            	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(ProSafeRom.SafeModePara), sizeof(TSafeModePara), (BYTE *)&SafeModePara );
								DAR = DAR_Success;
				            }
				        }
				    }
				}
			}
		}
		else
		{
			if( pData[0] == Array_698 )
			{
				if( pData[1] > MAX_SAFE_MODE_PARA_NUM )//设置的个数不能超过最大个数限制
				{
					DAR = DAR_OverRegion;
				}
				else
				{
					for( i = 0; i < pData[1]; i++ )
					{
						if( pData[2+8*i] == Structure_698 )
						{
							if( pData[3+8*i] == 2 )
							{
								if( pData[4+8*i] == OI_698 )
								{
									lib_ExchangeData((BYTE*)&SafeModePara.Sub[i].OI, (BYTE*)&pData[5+8*i], 2);
									if( pData[7+8*i] == Long_unsigned_698 )
									{
										DAR = DAR_Success;
										lib_ExchangeData((BYTE*)&SafeModePara.Sub[i].wSafeMode, (BYTE*)&pData[8+8*i], 2);
									}
									else
									{
										DAR = DAR_WrongType;
										break;
									}
								}
								else
								{
									DAR = DAR_WrongType;
									break;
								}
							}
							else
							{
								DAR = DAR_WrongType;
								break;
							}
						}
						else
						{
							DAR = DAR_WrongType;
							break;
						}
					}
				}

				if( DAR == DAR_Success )
				{
					SafeModePara.Num = pData[1]; 
					api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(ProSafeRom.SafeModePara), sizeof(TSafeModePara), (BYTE *)&SafeModePara );
				}
			}
	   }
	}
	else
	{
		DAR = DAR_WrongType;
	}

	eResultChoice = DLT698AddOneBuf(Ch,0x55, DAR);//是完整数据
    return eResultChoice;
}

//--------------------------------------------------
//功能描述:    设置esam参数
//         
//参数:      BYTE Ch[in]     通道选择
//         
//         BYTE *pOAD[in]  OAD数据指针
//         
//返回值:     eAPPBufResultChoice  返回添加Buf结果
//         
//备注内容:    无
//--------------------------------------------------
eAPPBufResultChoice SetRequestESAM( BYTE Ch, BYTE *pOAD )
{
	BYTE DAR,ClassAttribute,ClassIndex,Buf[10];
	BYTE *pData;
	TTwoByteUnion OI;
	eAPPBufResultChoice eResultChoice;
	WORD Result;
	
	pData = &pOAD[4];//获取数据指针
	lib_ExchangeData(OI.b,pOAD,2);//获取OI
	ClassAttribute = (pOAD[2]&0x1f);
	ClassIndex = pOAD[3];
	DAR = DAR_RefuseOp;		

    if( ClassIndex == 0 )
    {
    	if( (ClassAttribute == 0X0E)&&( CURR_COMM_TYPE( Ch ) != COMM_TYPE_TERMINAL ) )//设置红外认证时效
    	{
    	    if( pData[0] == Double_long_unsigned_698 )
    	    {
    	        lib_ExchangeData( Buf, (BYTE*)&pData[1], 4 );//进行数据倒序
				Result = api_WritePrePayPara( eIRTime, Buf );
			   
			   if( Result == TRUE )
               {
                    DAR = DAR_Success;
               }
    	    }
    	}	
    }
    
	eResultChoice = DLT698AddOneBuf(Ch,0x55, DAR);//是完整数据
	
    return eResultChoice;

}

//--------------------------------------------------
//功能描述:    设置参变量类参
//         
//参数:      BYTE Ch[in]     通道选择
//         
//         BYTE *pOAD[in]  OAD数据指针
//         
//返回值:     eAPPBufResultChoice  返回添加Buf结果        
//         
//备注内容:
//--------------------------------------------------
eAPPBufResultChoice SetRequestVariable( BYTE Ch, BYTE *pOAD )
{
	BYTE DAR;
	BYTE *pData;
	BOOL Result;
	TTwoByteUnion OI;
	eAPPBufResultChoice eResultChoice;
	
	pData = &pOAD[4];//获取数据指针
	lib_ExchangeData(OI.b,pOAD,2);//获取OI
	DAR = DAR_RefuseOp;


    if( OI.w == 0x2015 )//2015
    {
    	if( pOAD[2] == 4 )//属性4（电能表跟随上报模式字）∷= bit-string(SIZE(32))
    	{
        	DAR = SetRequestFollowReportMode( pData );
    	}
    	else if( pOAD[2] == 5 )//属性5 上报方式
    	{
			if( pData[0] == Enum_698 )
			{
				Result = api_SetEventReportMode( eReportMethod, OI.w, pData[1] );
				if(Result == FALSE )
				{
					DAR = DAR_HardWare;
				}
				else
				{
					DAR = DAR_Success;
				}
			}
    	}
    	else
    	{}

    }

    eResultChoice = DLT698AddOneBuf(Ch,0x55, DAR);//是完整数据
	
	return eResultChoice;
}

//--------------------------------------------------
//功能描述:    设置计量回路数
//         
//参数:      BYTE Ch[in]     通道选择
//         
//         BYTE *pOAD[in]  OAD数据指针
//         
//返回值:     eAPPBufResultChoice  返回添加Buf结果
//         
//备注内容:    无
//--------------------------------------------------
BYTE SetSampleChip( BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )
{
	BYTE DAR,Result;
	eAPPBufResultChoice eResultChoice;
	BYTE chipNo;
	
	if( (ClassAttribute != 2) || ( ClassIndex != 0 ) )
	{
		return DAR_WrongType;
	}	

	DAR = DAR_WrongType;

    if( ( ClassIndex == 0 ) && (ClassAttribute == 0x02) )
    {
	    if( pData[0] == Unsigned_698 )
	    {
			for(chipNo = 0; chipNo < SAMPLE_CHIP_NUM; chipNo++)
			{
				Result = api_SetSampleChip(pData[1], chipNo);
			}
		   
			if( Result == TRUE )
			{
				DAR = DAR_Success;
			}
	    }	
    }
	
    return DAR;

}
//--------------------------------------------------
//功能描述:  
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
BYTE  SetShakeTime(BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData)
{
	if ((ClassAttribute!=0x02)||(ClassIndex!=0x00)||((*pData)!=0x06))
	{
		return DAR_WrongType;
	}
	else
	{
		lib_ExchangeData(pData+1, pData+1,4);
		if (api_WriteDIPara(0,pData+1)==FALSE)
		{
			return DAR_RefuseOp;
		}
		else
		{
			return DAR_Success;
		}

	}
	
}
//--------------------------------------------------
//功能描述:    设置参变量类参
//         
//参数:      BYTE Ch[in]     通道选择
//         
//         BYTE *pOAD[in]  OAD数据指针
//         
//返回值:     eAPPBufResultChoice  返回添加Buf结果        
//         
//备注内容:
//--------------------------------------------------
eAPPBufResultChoice SetRequestParameter( BYTE Ch, BYTE *pOAD )
{
	BYTE DAR,ClassAttribute,ClassIndex;
	BYTE *pData;
	TTwoByteUnion OI;
	eAPPBufResultChoice eResultChoice;
	
	pData = &pOAD[4];//获取数据指针
	lib_ExchangeData(OI.b,pOAD,2);//获取OI
	ClassAttribute = (pOAD[2]&0x1f);
	ClassIndex = pOAD[3];
	DAR = DAR_WrongType;
	
	switch(OI.w)
	{
	case 0x4000://日期时间
		DAR = SetRTC( ClassAttribute, ClassIndex, pData);
		break;
	case 0x4001://通信地址
	case 0x4002://表号
	case 0x4003://客户编号
		DAR = SetCommAddrOrMeterNoOrCustomCode( OI.w, ClassAttribute,ClassIndex, pData, Ch);
		break;
	case 0x4004://设备地理位置	
		DAR = SetPsitionInfo( ClassAttribute, ClassIndex,  pData);
		break;
	// case 0x4007://lcd显示参数
	// 	DAR = SetLCDPara( ClassAttribute, ClassIndex, pData);
	// 	break;
	case 0x4008://时区切换时间
	case 0x4009://时段表切换时间
//	#if( PREPAY_MODE == PREPAY_LOCAL )//本地预付费支持
//	case 0x400A://费率切换时间
//	case 0x400B://阶梯切换时间
//	#endif
        DAR = SetSwitchTime( 0X00, OI.w, ClassAttribute, ClassIndex, pData);
			
		break;
	case 0x400c://时区时段数
		DAR = Set_Timezone_TimeInterval_Num( ClassAttribute,  ClassIndex, pData);
		break;
    #if( PREPAY_MODE == PREPAY_LOCAL )//卡表支持设置
    case 0x400D://阶梯数
	    DAR = Set_LadderNum( ClassAttribute,  ClassIndex, pData);
	    break;
	#endif
	case 0x400f://密钥总条数
		DAR = SetEsamKeyTotalNum( ClassAttribute,  ClassIndex, pData);
		break;
	case 0x4010://计量回路数
		DAR = SetSampleChip( ClassAttribute,  ClassIndex, pData);
		break;
#if( MAX_PHASE != 1 )	
	case 0x4100://最大需量周期
	case 0x4101://滑差时间
		DAR = SetDemandPara( OI.w, ClassAttribute, ClassIndex, pData);
		break;		
#endif//#if( MAX_PHASE != 1 )	
	case 0x4104://额定电压   //资产管理编码单独设置
	case 0x4105://额定电流/基本电流
	case 0x4106://最大电流
	case 0x4107://有功准确度等级
	case 0x4108://无功准确度等级
	case 0x4109://电能表有功常数
	case 0x410A://电能表无功常数
	case 0x410B://电能表型号
	case 0x4111://软件备案号
		DAR = SetMeterBasicPara( OI.w, ClassAttribute, ClassIndex, pData);
		break;
	case 0x4011://公共假日表
		DAR = SetTimeHolidayTable( ClassAttribute,  ClassIndex, pData);
		break;
	case 0x4012://周休日特征字
		DAR = SetWeekStatus( ClassAttribute, ClassIndex, pData);
		break;
	case 0x4013://周休日采用的日时段表号
		DAR = SetWeekSeg( ClassAttribute, ClassIndex, pData);
		break;

	case 0x4014://当前套时区
		DAR = SetTimezoneTable( 0x01, ClassAttribute, ClassIndex,  pData);
		break;
		
	case 0x4015://备用套时区
		DAR = SetTimezoneTable( 0x81,ClassAttribute, ClassIndex,  pData);
		break;
		
	case 0x4016://当前套时段表
		DAR = SetTimeTable( 0x03, ClassAttribute, ClassIndex,  pData);
		break;
		
	case 0x4017://备用套时段表
		DAR = SetTimeTable( 0x83, ClassAttribute, ClassIndex,  pData);
		break;

/*	case 0x401C://电流互感器变比
	case 0x401D:
	    if( api_GetSysStatus( eSYS_STATUS_INSIDE_FACTORY ) == TRUE )
	    {
            DAR = SetPTCT( OI.w, ClassAttribute, ClassIndex, pData);
	    }
	    else
	    {
            DAR = DAR_RefuseOp;
	    }
		
		break;*/
		
    #if( PREPAY_MODE == PREPAY_LOCAL )//卡表支持设置
	//case 0x401e://报警金额限值
    case 0x401f://其他金额限值
        DAR = SetLimitMoney( 0x00, OI.w, ClassAttribute, ClassIndex, pData );
	    break;
	#endif
	
	case 0x4030://电压合格率参数
		DAR = SetVoltageRatePara( OI.w, ClassAttribute,  ClassIndex, pData);
		break;
	case 0x4103:
		DAR = SetPropertyCode( ClassAttribute, ClassIndex, pData);
		break;
		
	case 0x4112://有功组合模式字
#if( MAX_PHASE != 1 )
	case 0x4113://无功组合模式字1
	case 0x4114://无功组合模式字2
#endif//#if( MAX_PHASE != 1 )	
		DAR = SetCOMStatus( OI.w, ClassAttribute, ClassIndex, pData);
		break;
#if( MAX_PHASE != 1 )	
	case 0x4117://期间需量冻结周期
		DAR = SetIntervalDemandFreezingPeriod( ClassAttribute,  ClassIndex, pData);	
		break;	
#endif//#if( MAX_PHASE != 1 )	
	case 0x4116://结算日
		DAR = SetBillingPara( ClassAttribute, ClassIndex, pData);
		break;
	
	case 0x4300://电气设备
		DAR = SetMeter( ClassAttribute,  ClassIndex, pData);
		break;
		
	case 0x4401://应用连接密码
		DAR = SetPassWord( ClassAttribute, ClassIndex, pData );
		break;
	case 0x4500://公网通信模块1
	case 0x4501://公网通信模块2
		DAR = SetGPRSdBm( ClassAttribute, ClassIndex, pData );
		break;
	case 0x4908:
		DAR = SetShakeTime( ClassAttribute, ClassIndex, pData );
        break;
    #if (SEL_TOPOLOGY == YES )	    
    case 0x4E06:
		DAR = api_SetTopoPara698( ClassAttribute, ClassIndex, pData );
        break;
    #endif
    #if (SEL_SEARCH_METER == YES )	
    case 0x6002://搜表参数
        DAR = api_SetSchedMeter698( ClassAttribute, ClassIndex, pData );
        break;
    #endif
	default:
		DAR = DAR_WrongType;
		break;		
	}
	
	eResultChoice = DLT698AddOneBuf(Ch,0x55, DAR);//是完整数据
	
	return eResultChoice;
}
//--------------------------------------------------
//功能描述:    设置参数的普通请求
//         
//参数:      BYTE Ch[in]     通道选择
//         
//         BYTE *pOAD[in]  OAD数据指针
//         
//返回值:     eAPPBufResultChoice  返回添加Buf结果        
//         
//备注内容:    无
//--------------------------------------------------
eAPPBufResultChoice SetRequestNormal( BYTE Ch,BYTE *pOAD)
{
	TTwoByteUnion OI;
	eAPPBufResultChoice eResultChoice;
	BYTE OAD[4],dar;
	WORD DataLen;
	DWORD oadd=0;


    if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return eAPP_ERR_RESPONSE;
    }
    
	lib_ExchangeData(OI.b,pOAD,2);
	lib_ExchangeData((BYTE*)&oadd,pOAD,4);
	eResultChoice = DLT698AddBuf(Ch,0, pOAD, 4);//添加OAD数据 不是完整数据
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	
	memcpy( OAD, pOAD, 4);//保存编程OAD;
	
    DataLen = GetProtocolDataLen( pOAD+4);//获取Data长度 DataLen的错误长度在后面进行了判断

	if( APPData[Ch].TimeTagFlag == eTime_Invalid )//时间标签错误
    {
        eResultChoice = DLT698AddOneBuf( Ch, 0x55, DAR_TimeStamp);
    }
	else if(JudgeTaskAuthority( Ch, eSET_MODE, pOAD) == FALSE)//判断安全模式参数和是否建立应用连接
	{	
		if( api_GetRunHardFlag(eRUN_HARD_MAC_ERR_FLAG) == TRUE )
		{
			eResultChoice = DLT698AddOneBuf( Ch, 0x55, DAR_HangUp);
		}
		else
		{
			eResultChoice = DLT698AddOneBuf( Ch, 0x55, DAR_PassWord);
		}
	}	
    else if( (OI.w >= 0x0000) && (OI.w <= 0x2133) )//电能类、需量类、变量类 不支持设置 统一返回拒绝操作
    {
        eResultChoice = SetRequestVariable( Ch, pOAD );
    }
	else if( (OI.w >= 0x3000) && (OI.w <= 0x330e) 
            #if( SEL_TOPOLOGY == YES )
            || (OI.w ==0x3E03)
            #endif
     )//设置事件类对象
	{
		eResultChoice = SetRequestEvent( Ch, pOAD );
	}

	else if( ((OI.w >= 0x4000) && (OI.w <= 0x4517)) ||(OI.w == 0x4908)
	  #if (SEL_SEARCH_METER == YES )
	  ||(OI.w == 0x6002)
	  #endif
      #if (SEL_TOPOLOGY == YES )
	  ||(OI.w == 0x4E06)
	  #endif
	)//设置参变量 
	{
		eResultChoice = SetRequestParameter( Ch, pOAD );
	}
	else if( (OI.w >= 0x5000) && (OI.w <= 0x5011))//设置冻结
	{
		eResultChoice = SetRequestFreeze( Ch, pOAD );
	}
	else if(OI.w == 0x6000)//设置采集
	{
		Class11_Set(oadd,pOAD+4,(DAR*)&dar);
		eResultChoice = DLT698AddOneBuf(Ch,0x55, dar);
	}
	else if( (OI.w >= 0x8000) && (OI.w <= 0x8001))//设置控制参数
	{
		eResultChoice = SetRequestControl( Ch, pOAD );
	}
	#if(SEL_ESAM_TYPE != 0)
	else if( OI.w == 0xf100 )//设置ESAM相关参数
	{
		eResultChoice = SetRequestESAM( Ch, pOAD );
	}
	else if( OI.w == 0xf101 )//设置安全模式参数
	{
		eResultChoice = SetRequestSafeMode( Ch, pOAD );
	}
	#endif
	else if( OI.w == 0xF203 )//设置开关量输入标志
	{
		eResultChoice = SetDIInputFlag(Ch, pOAD );
	}
	// else if( OI.w == 0xF20B )//设置外部设备
	// {
	// 	eResultChoice = SetRequestInOuputPara(Ch, pOAD );
	// }
	else if( OI.w == 0xf221 )//设置充电桩
	{
		eResultChoice = DLT698AddOneBuf(Ch,0x55, SetCharging( pOAD, pOAD+4));
	}
	// else if( (OI.w >= 0xF300) && (OI.w <= 0xF301))//设置液晶
	// {
	// 	eResultChoice = SetRequestLCD( Ch, pOAD);
	// }
	else//其他
	{
		eResultChoice = DLT698AddOneBuf(Ch,0x55, DAR_Undefined);//是完整数据
	}
	
	if( eResultChoice == eADD_OK )
	{
	    if( APPData[Ch].pAPPBuf[(APPData[Ch].APPCurrentBufLen.w-1)] == DAR_Success )//将Buf回退1个字节 判断DAR 设置成功记录编程记录
	    {
            api_SaveProgramRecord( EVENT_START,0x51, OAD);
	    }		
	}
	//为避免现场一直设置公网通信模块信号强度，设置公网通信模块不记录自由事件
	if((OI.w != 0x4500) && (OI.w != 0x4501))
	{
		api_WriteFreeEvent( (EVENT_PROGRAM_698 << 8) + OAD[2], OI.w );
	}
	
	if( DataLen == 0x8000 )//长度不能正常解析，不再进行list数据处理
	{
        return eAPP_RETURN_DATA;
	}
	
	APPData[Ch].BufDealLen.w += (DataLen+4);
	return eResultChoice;
}
//--------------------------------------------------
//功能描述:    多个设置请求
//         
//参数:      BYTE Ch[in]  通道选择
//         
//返回值:     eAPPBufResultChoice 返回添加Buf结果
//         
//备注内容:    无
//--------------------------------------------------
eAPPBufResultChoice SetRequestNormalList( BYTE Ch)
{
	BYTE i,LenOffest;
	WORD NoAddress;
	eAPPBufResultChoice eResultChoice;
	TTwoByteUnion Num,No;
	
	if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return eAPP_ERR_RESPONSE;
    }
    
    LenOffest = Deal_698DataLenth( (BYTE*)&LinkData[Ch].pAPP[APPData[Ch].BufDealLen.w], Num.b, ePROTOCOL_TYPE ); 
    APPData[Ch].BufDealLen.w +=LenOffest;

	No.w = 0;

	eResultChoice = DLT698AddOneBuf(Ch,0, Num.b[0] );//添加个数 产生后续帧不正确
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	
	NoAddress = APPData[Ch].APPCurrentBufLen.w;
	
	for(i=0;i<Num.w;i++)
	{
		eResultChoice = SetRequestNormal( Ch, &LinkData[Ch].pAPP[APPData[Ch].BufDealLen.w]);
		
		if( eResultChoice != eADD_OK )//添加不成功就返回
		{
			return eResultChoice;
		}
		
		No.w++; //sequence of set_result
		
		if( APPData[Ch].BufDealLen.w >= LinkData[Ch].pAPPLen.w )//已处理数据大于下发的数据进行返回，避免错误报文的干扰
		{
			break;
		}
	}
	
	JudgeAppBufLen(Ch);
	
	Deal_698SequenceOfNumData( Ch, NoAddress, No.b);
	
	return eResultChoice;
}
//--------------------------------------------------
//功能描述:    设置后读取-列表
//         
//参数:      BYTE Ch[in]  通道选择
//         
//返回值:     eAPPBufResultChoice 返回添加Buf结果
//         
//备注内容:    无
//--------------------------------------------------
eAPPBufResultChoice SetThenGetRequestNormalList( BYTE Ch )
{
	BYTE i,LenOffest;
	WORD NoAddress;
	eAPPBufResultChoice eResultChoice;
	TTwoByteUnion Num,No;
	
	if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return eAPP_ERR_RESPONSE;
    }
    
    LenOffest = Deal_698DataLenth( (BYTE*)&LinkData[Ch].pAPP[APPData[Ch].BufDealLen.w], Num.b, ePROTOCOL_TYPE ); 
    APPData[Ch].BufDealLen.w +=LenOffest;
	No.w = 0;
	
	eResultChoice = DLT698AddOneBuf(Ch,0, Num.b[0] );//添加个数 产生后续帧不正确
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}

	NoAddress = APPData[Ch].APPCurrentBufLen.w;
	
	for(i=0;i<Num.w;i++)
	{
		//设置数据
		eResultChoice = SetRequestNormal( Ch,&LinkData[Ch].pAPP[APPData[Ch].BufDealLen.w] );//
		if( eResultChoice != eADD_OK )//添加不成功就返回
		{
			return eResultChoice;
		}
		if( APPData[Ch].BufDealLen.w >= LinkData[Ch].pAPPLen.w )//已处理数据大于下发的数据进行返回，避免错误报文的干扰
		{
			break;
		}
		//读取数据
		eResultChoice= GetRequestNormal( Ch, &LinkData[Ch].pAPP[APPData[Ch].BufDealLen.w]);
		if( eResultChoice != eADD_OK )//添加不成功就返回
		{
			return eResultChoice;
		}
		
		APPData[Ch].BufDealLen.w += 1;//读取延时
		 
		No.w ++;//sequence of set_then_get_result
		
		if( APPData[Ch].BufDealLen.w >= LinkData[Ch].pAPPLen.w )//已处理数据大于下发的数据进行返回，避免错误报文的干扰
		{
			break;
		}
	}
	
	JudgeAppBufLen(Ch);
	
	Deal_698SequenceOfNumData( Ch, NoAddress, No.b);
	
    return eResultChoice;
}

//--------------------------------------------------
//功能描述:    esam数据更新（更新到epprom中）
//         
//参数:      BYTE* pOAD[in]    OAD指针
//         
//         BYTE *pData       数据指针
//
//返回值:     BYTE DAR结果
//         
//备注内容:    无
//--------------------------------------------------
BYTE EsamDataUpdate( BYTE* pOAD, BYTE *pData )
{
    WORD OI,Result,Type;
    BYTE ClassAttribute,ClassIndex,DAR;
    
    DAR = DAR_RefuseOp;//AR 成功   
    lib_ExchangeData((BYTE*)&OI,pOAD,2);//获取OI
	ClassAttribute = (pOAD[2]&0x1f);
	ClassIndex = pOAD[3];

    switch( OI )
    {
        case 0x4002://表号
        case 0x4003://客户编号
            if( api_GetRunHardFlag(eRUN_HARD_COMMON_KEY) == FALSE )
		    {
                return DAR_PassWord;
		    }

            if( OI == 0x4002 )
            {
                if( pData[0] != 8 )//数据长度必须为8
                {
                    return DAR_RefuseOp;
                }
                Type = eMeterMeterNo;
                pData = pData+2;
            }
            else
            {                
                if( pData[0] != 6 )//数据长度必须为6
                {
                    return DAR_RefuseOp;
                }
                
                Type = eMeterCustomCode;
            }
            
            //验证esam
            Result = api_AuthDataSID( ProtocolBuf );
			if( Result == FALSE )
			{
			   return DAR_EsamFial;
			}

            Result =api_ProcMeterTypePara( WRITE, Type, pData+1 );//ESAM表号8个字节 epprom 6个字节
            if( Result != FALSE )
            {
                DAR = DAR_Success;
            }
            break;
        #if( PREPAY_MODE == PREPAY_LOCAL)
        case 0x400A://备用套分时费率切换时间
        case 0x400B://备用套阶梯电价切换时间
            DAR = SetSwitchTime( 0x55, OI, ClassAttribute, ClassIndex, pData );
            break;
        case 0x401e://报警金额限制
            DAR = SetLimitMoney( 0x55, OI, ClassAttribute, ClassIndex, pData );
            break;
		case 0x4019://备用套电价
            DAR = SetBackupRate( ClassAttribute, ClassIndex, pData );
            break;
        case 0x401B://备用套阶梯
            DAR = SetBackupLadder( ClassAttribute, ClassIndex, pData );
            break;
        #endif 
            
        case 0x401c://电流互感器变比
        case 0x401d://电压互感器变比
            DAR = SetPTCT( OI, ClassAttribute, ClassIndex, pData);
            break;
            
        default:
        	DAR = DAR_Success;
            break;
    }
    
    if( DAR == DAR_Success )
    {
        api_SaveProgramRecord( EVENT_START, 0x53, pOAD);//数据更新保存OAD 
    }

    return DAR;
}

//--------------------------------------------------
//功能描述:    远程控制操作
//         
//参数:      BYTE Ch[in]        通道选择
//         
//         BYTE MethodNo[in]  方法号
//         
//         BYTE *pData[in]    数据指针
//         
//返回值:     BYTE DAR结果       
//         
//备注内容:    无
//--------------------------------------------------
BYTE ActionRemoteControl(BYTE Ch, BYTE MethodNo,BYTE *pData )
{
	BYTE DAR,RelayOpType,RelayWaitOffTime;
	WORD Result,Offest;
	TTwoByteUnion RelayOffWarnTime,Len;
	
	Result = 0x8000;
	DAR = DAR_WrongType;
	RelayWaitOffTime = 0;
	RelayOffWarnTime.w = 0;
	RelayOpType = 0;
	
	switch( MethodNo )
	{
		case 127://触发报警
		    if( pData[0] == NULL_698 )//数据为Null
		    {
                RelayOpType = eCMD_InAlarm;
                DAR = DAR_Success;
                if( (APPData[Ch].eSafeMode >= eEXPRESS_MAC) && (APPData[Ch].eSafeMode <= eSECRET_MAC) )//权限限定在明文+mac，密文，密文+mac
                {
                    FactoryTime = (10*60);
                }
		    }
			break;
		case 128://解除报警
		    if( pData[0] == NULL_698 )//数据为Null
		    {
				RelayOpType = eCMD_OutAlarm;
			    DAR = DAR_Success;	
				FactoryTime = 0;    
		    }
			break;
		case 129://跳闸
			if( pData[0] == Array_698 )
			{
				if( pData[1] == 0x01)
				{
					if( pData[2] == Structure_698 )
					{
						if( pData[2+7] == Unsigned_698 )//告警延时
						{
							RelayWaitOffTime = pData[2+8];
							if( pData[2+9] == Long_unsigned_698 )//限电延时
							{
								RelayOffWarnTime.b[1] = pData[2+10] ;
								RelayOffWarnTime.b[0] = pData[2+11] ;
								if( RelayOffWarnTime.w == 0)//限电延时为零则为永久限电
								{
									RelayOpType = eCMD_RelayOff;
									DAR = DAR_Success;
								}
								else if( pData[2+12] == Boolean_698 )//自动合闸
								{
									if( pData[2+13] ==  1 )//自动合闸
									{
										RelayOpType = eCMD_AdvOff1;//预跳闸1
									}
									else
									{
										RelayOpType = eCMD_AdvOff2;//预跳闸2
									}

									DAR = DAR_Success;
								}
							}
						}
					}
				}
			}
		
			break;
		case 130://合闸                  
			if( pData[0] == Array_698 )
			{
				if( pData[1] == 0x01 )
				{
	                if( pData[2] == Structure_698 )
	                {
	                	if( pData[3] == 2 )// 2个对象属性
	                	{
							if( pData[3+6] == Enum_698 )
							{
								if( pData[4+6] == 0)
								{
									RelayOpType = eCMD_IndirOn;
								}
								else
								{
									RelayOpType = eCMD_DirectOn;
								}
								DAR = DAR_Success;
							}
                    	}
                    }
				}
			}			
			break;

		case 131://电表明文跳合闸
			if( pData[0] == Array_698 )
			{
				if( pData[1] == 01 )
				{
					if( pData[2] == Structure_698 )
					{
						if( pData[3] == 3 )// 3个元素
						{
							if( pData[3+6] == Enum_698 )
							{
								if( pData[3+7] == 0 )
								{
									RelayOpType = eCMD_IndirOn;
								}
								else
								{
									RelayOpType = eCMD_DirectOn;
								}

								if(  pData[3+8] == Visible_string_698 )
								{
									Offest = Deal_698DataLenth( (BYTE*)&pData[3+9], Len.b, ePROTOCOL_TYPE );
									Result = Judge698PassWord( (BYTE*)&pData[3+9+Offest] );
									if( Result == FALSE )
									{
										//置继电器错误状态字
										api_SetRelayRelayErrStatus( 0x04 );
										DAR = DAR_PassWord;
									}
									else
									{
                                        DAR = DAR_Success;
									}
								}
							}
						}
					}
				}
			}
			break;
		default:
			DAR = DAR_Undefined;//AR 硬件时效	
			break;
	}

	if( DAR == DAR_Success )
	{	
		Result = api_Set_RemoteRelayCmd( RelayOpType, RelayWaitOffTime, RelayOffWarnTime.w);
		if( Result & 0x8000 )
		{
			api_WriteFreeEvent(EVENT_RELAY_ERR, Result);
			DAR = DAR_RefuseOp;
		}
		else
		{
			DAR = DAR_Success;
		}
	}

	return DAR;
}
#if (SEL_SEARCH_METER == YES )
//--------------------------------------------------
//功能描述:    搜表操作
//         
//参数:      BYTE Ch[in]        通道选择
//         
//         BYTE MethodNo[in]  方法号
//         
//         BYTE *pData[in]    数据指针
//         
//返回值:     BYTE DAR结果       
//         
//备注内容:    无
//--------------------------------------------------
BYTE ActionSchelMeter( BYTE Ch,BYTE MethodNo,BYTE *pData )
{
	BYTE DAR,RelayOpType,i;
	WORD Result;
    WORD SearchMaxtime;
	
	Result = 0x8000;
	DAR = DAR_WrongType;

	switch( MethodNo )
	{
	case 127://搜表启动(搜表时长)
	    if( pData[0] == Long_unsigned_698 )//数据为Long_unsigned_698
	    {
            lib_ExchangeData((BYTE*)&SearchMaxtime,&pData[1],2);
            if(SKMeter.byAutoCheck != 0)
            {
                api_StopSchMeter(); 
            }
            api_StartSearchMeter(SearchMaxtime);
            DAR = DAR_Success;
	    }
		break;
	
	case 128://清空搜表结果
		if( pData[0] == NULL_698 )//数据为Null
		{
            if(SKMeter.byAutoCheck != 0)
            {
                api_StopSchMeter(); 
            }
			api_ClearSchMeter();
			DAR = DAR_Success;
		}
		break;

	case 129://清空跨台区搜表结果
		if( pData[0] == NULL_698 )//数据为Null
	    {
			//暂时未做
			DAR = DAR_Undefined;
	    }
		break;

	default:
		break;
	}

	return DAR ;
}
#endif //#if (SEL_SEARCH_METER == YES )
//--------------------------------------------------
//功能描述:    远程保电操作
//         
//参数:      BYTE Ch[in]        通道选择
//         
//         BYTE MethodNo[in]  方法号
//         
//         BYTE *pData[in]    数据指针
//         
//返回值:     BYTE DAR结果       
//         
//备注内容:    无
//--------------------------------------------------
BYTE ActionPowerSupply( BYTE Ch,BYTE MethodNo,BYTE *pData )
{
	BYTE DAR,RelayOpType;
	WORD Result;
	
	Result = 0x8000;
	DAR = DAR_WrongType;

	if( APPData[Ch].TimeTagFlag == eNO_TimeTag )//如果无时间标签 返回时间标签无效
	{
		return DAR_TimeStamp;
	}
	
	switch( MethodNo )
	{
	case 127://触发保电
	    if( pData[0] == NULL_698 )//数据为Null
	    {
            RelayOpType = eCMD_InKeep;
            DAR = DAR_Success;
	    }
		break;
	
	case 128://解除保电
		if( pData[0] == NULL_698 )//数据为Null
	    {
            RelayOpType = eCMD_OutKeep;
            DAR = DAR_Success;
	    }
		break;

	default:
		break;
	}
	
	if( DAR == DAR_Success)
	{	
		Result = api_Set_RemoteRelayCmd( RelayOpType, 0, 0);
		if(  (Result & 0x8000) )
		{
			api_WriteFreeEvent(EVENT_RELAY_ERR, Result);
			DAR = DAR_HardWare;
		}
		else
		{
			DAR = DAR_Success;
		}
	}

	return DAR ;
}
//--------------------------------------------------
//功能描述:    操作控制请求
//         
//参数:      BYTE Ch[in]          通道选择
//         
//         BYTE *pOMD[in]       方法指针
//         
//返回值:     eAPPBufResultChoice  返回添加Buf结果
//         
//备注内容:    无
//--------------------------------------------------
eAPPBufResultChoice ActionRequestControl( BYTE Ch, BYTE *pOMD)
{
	TTwoByteUnion OI;
	BYTE DAR,MethodNo;
	BYTE *pData;
	eAPPBufResultChoice eResultChoice;
	
	lib_ExchangeData(OI.b,pOMD,2);//取oi
	MethodNo = pOMD[2];//取方法
	pData = &pOMD[4];//取data
	DAR = DAR_WrongType;//默认失败

	if( APPData[Ch].TimeTagFlag == eNO_TimeTag )
	{
		api_SetRelayRelayErrStatus( 0x10 );
		DAR = DAR_TimeStamp;//时间标签错误

		api_WriteFreeEvent(EVENT_RELAY_ERR, R_ERR_TIME);
	}
    else
    {
        if( OI.w == 0x8000 )
    	{
    		DAR = ActionRemoteControl( Ch, MethodNo, pData);
    	}
    	else
    	{
    		DAR = ActionPowerSupply( Ch, MethodNo, pData);
    	}
    }

	eResultChoice = DLT698AddOneBuf(Ch,0x00, DAR);//添加DAR
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	eResultChoice = DLT698AddOneBuf(Ch,0x55, 00);//OPTIONAL NO CHOICE DATA 
	return eResultChoice;
	
}
#if (SEL_SEARCH_METER == YES )
//--------------------------------------------------
//功能描述:    操作搜表控制请求
//         
//参数:      BYTE Ch[in]          通道选择
//         
//         BYTE *pOMD[in]       方法指针
//         
//返回值:     eAPPBufResultChoice  返回添加Buf结果
//         
//备注内容:    无
//--------------------------------------------------
eAPPBufResultChoice ActionRequestSchelControl( BYTE Ch, BYTE *pOMD)
{
	TTwoByteUnion OI;
	BYTE DAR,MethodNo;
	BYTE *pData;
	eAPPBufResultChoice eResultChoice;
	
	lib_ExchangeData(OI.b,pOMD,2);//取oi
	MethodNo = pOMD[2];//取方法
	pData = &pOMD[4];//取data
	DAR = DAR_WrongType;//默认失败

//	zh添加20230505 ，送测屏蔽
//	if( APPData[Ch].TimeTagFlag == eNO_TimeTag )
//	{
//		api_SetRelayRelayErrStatus( 0x10 );
//		DAR = DAR_TimeStamp;//时间标签错误
//	}
//    else
    {
		DAR = ActionSchelMeter( Ch, MethodNo, pData);
    }

	eResultChoice = DLT698AddOneBuf(Ch,0x00, DAR);//添加DAR
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	eResultChoice = DLT698AddOneBuf(Ch,0x55, 00);//OPTIONAL NO CHOICE DATA 
	return eResultChoice;
	
}
#endif //#if (SEL_SEARCH_METER == YES )
//--------------------------------------------------
//功能描述:    操作设备类对象
//         
//参数:      BYTE Ch[in]          通道选择
//         
//         BYTE *pOMD[in]       方法指针
//         
//返回值:     eAPPBufResultChoice  返回添加Buf结果 
//         
//备注内容:    无
//--------------------------------------------------
eAPPBufResultChoice ActionRequestMeter( BYTE Ch, BYTE *pOMD)
{
	TTwoByteUnion OI;
	BYTE DAR;
	BYTE *pData;
	eAPPBufResultChoice eResultChoice;
	TFourByteUnion OMD;
	
	DAR = DAR_WrongType;
    lib_ExchangeData(OI.b,pOMD,2);//取oi
	pData = &pOMD[4];//取data
	DAR = DAR_WrongType;//默认失败
	
	if( APPData[Ch].TimeTagFlag == eNO_TimeTag )//清零支持带时间标签的清零
	{
		DAR = DAR_TimeStamp;//时间标签无效
	}
	else if( api_GetRunHardFlag(eRUN_HARD_MAC_ERR_FLAG) == TRUE )//挂起不允许进行远程控制
	{
		DAR = DAR_HangUp;
	}
	else
    {
        if( pData[0] == NULL_698 )//数据为NULL
        {
            switch( pOMD[2] )
            {
                case 3://数据初始化
                	if( api_GetRunHardFlag(eRUN_HARD_COMMON_KEY) == FALSE )//私钥下不允许操作
                	{
						DAR = DAR_RefuseOp;
						break;
                	}
                	
                	#if( PREPAY_MODE == PREPAY_LOCAL )
                	if( api_GetSysStatus(eSYS_STATUS_INSIDE_FACTORY) == FALSE )//本地费控表只有厂内模式才可以使用电表清零命令，即4300的方法3:数据初始化
                	{
                		DAR = DAR_RefuseOp;
                		break;
                	}	
                	#endif
                	
                    api_SetClearFlag( eCLEAR_METER, 1 );
                    api_WriteFreeEvent(EVENT_DATA_INIT_698,OI.w);
					api_SavePrgOperationRecord( ePRG_CLEAR_METER_NO );
                    //默认延时500ms~1s，这样实际延时范围500~1500ms,1200bps 连续发送20字节时间为 183ms
                    DAR = DAR_Success;
                break;
                
                case 4://恢复出厂参数
                    DAR = DAR_RefuseOp;//默认失败
                break;
                
                case 5://事件初始化
                    memcpy(OMD.b,pOMD,4);//获取OAD
                    api_SetClearFlag( eCLEAR_EVENT, 1 );
                    //默认延时500ms~1s，这样实际延时范围500~1500ms,1200bps 连续发送20字节时间为 183ms
                    api_WritePreProgramData( 1, OMD.d );
                    api_WriteFreeEvent(EVENT_EVENT_INIT_698,OI.w);
					api_SavePrgOperationRecord( ePRG_CLEAR_EVENT_NO );
                    DAR = DAR_Success;
                break;
                
                case 6://需量初始化
                    #if( MAX_PHASE != 1)        
					api_SavePrgOperationRecord( ePRG_CLEAR_MD_NO );
                    api_SetClearFlag( eCLEAR_DEMAND, 1 );
                    //默认延时500ms~1s，这样实际延时范围500~1500ms,1200bps 连续发送20字节时间为 183ms
                    DAR = DAR_Success ;
                    #else
                    DAR = DAR_Undefined ;
                    #endif
                    api_WriteFreeEvent(EVENT_DEMAND_INIT_698,OI.w);
                break;
                
                default:
                     DAR = DAR_WrongType;//默认失败
                break;
            }
        }

    }   
	
	eResultChoice = DLT698AddOneBuf(Ch,0x00, DAR);//添加DAR
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	eResultChoice = DLT698AddOneBuf(Ch,0x55, 00);//OPTIONAL NO CHOICE DATA 
	
	return eResultChoice;	
}
//--------------------------------------------------
//功能描述:    698操作请求事件
//         
//参数:      BYTE Ch[in]          通道选择
//         
//         BYTE *pOMD[in]       方法指针
//         
//返回值:     eAPPBufResultChoice  返回添加Buf结果 
//         
//备注内容:    无
//--------------------------------------------------
eAPPBufResultChoice ActionRequestEvent( BYTE Ch, BYTE *pOMD)
{
	TTwoByteUnion OI;
	TFourByteUnion OAD,OMD;
	BYTE DAR,MethodNo;
	BYTE *pData;
	WORD Result;
	eAPPBufResultChoice eResultChoice;
	
	lib_ExchangeData(OI.b,pOMD,2);//取oi
	MethodNo = pOMD[2];//取方法
	pData = &pOMD[4];//取data
	DAR = DAR_WrongType;//默认失败
	Result = FALSE;
	
	switch ( MethodNo )
	{
		case  1://复位
			if( APPData[Ch].TimeTagFlag == eNO_TimeTag )//清零支持带时间标签的清零
        	{
        		DAR = DAR_TimeStamp;//时间标签无效
        	}       	
		    else if( (OI.w == 0x3015) || (OI.w == 0x3013) )//事件清零和电表清零不允许单独·清零
            {
                DAR = DAR_RefuseOp;
            }
            else if( pData[0] == Integer_698 )//数据为Integer_698
            {
                DAR = DAR_Success;
                memcpy(OMD.b,pOMD,4);//获取OAD
                Result = api_ClrSepEvent(OI.w);
                api_WritePreProgramData( 1, OMD.d );
				api_SavePrgOperationRecord( ePRG_CLEAR_EVENT_NO );
				//事件复位需要设置新增事件列表
				SetReportIndex( eSUB_EVENT_PRG_CLEAR_EVENT, EVENT_START+EVENT_END );
            }
			break;
		case  2://执行
			DAR =DAR_WrongType;
			break;
		case  3://触发冻结		
			break;
		case  4://添加一个冻结对象属性		
			if( pData[0] == OAD_698 )
			{
				memcpy(OAD.b,&pData[1],4);//取OAD
				DAR = DAR_Success;
			}
			else
			{
				break;
			}
			
			if((OI.w == 0x3013) && (api_GetRunHardFlag(eRUN_HARD_COMMON_KEY) == FALSE) )//私钥状态下不允许更改事件清零关联对象属性表
            {
                DAR = DAR_RefuseOp; 
            }
			else if( DAR == DAR_Success )
			{
				Result = api_AddEventAttribute(OI.w, OAD.b);
			}
			break;

		case  5://删除一个冻结对象属性		
			if( pData[0] == OAD_698 )
			{
				memcpy(OAD.b,&pData[1],4);//取OAD
				DAR = DAR_Success;
			}
			else
			{
				break;
			}
            if((OI.w == 0x3013) && (api_GetRunHardFlag(eRUN_HARD_COMMON_KEY) == FALSE) )//私钥状态下不允许更改事件清零关联对象属性表
            {
                DAR = DAR_RefuseOp; 
            }
			else if( DAR == DAR_Success )
			{
				Result = api_DeleteEventAttribute(OI.w,OAD.b );
			}
			break;		
		default:
			break;
	}
	
	if( DAR == DAR_Success )//数据成功后 判断操作结果
	{
		if( Result == FALSE )
		{
			DAR = DAR_RefuseOp;
		}
		else
		{
			DAR = DAR_Success;
		}
	}

	eResultChoice = DLT698AddOneBuf(Ch,0x00, DAR);//添加DAR
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	eResultChoice = DLT698AddOneBuf(Ch,0x55, 00);//OPTIONAL NO CHOICE DATA 
	
	return eResultChoice;
}
//--------------------------------------------------
//功能描述:    698操作添加一个属性对象请求
//         
//参数:      BYTE Type[in]    数据OI
//         
//         BYTE *pData[in]  数据指针
//         
//返回值:     BYTE DAR结果
//         
//备注内容:    无
//--------------------------------------------------
BYTE ActionAddFreeze( WORD Type,BYTE *pData )
{
	BYTE DAR,Buf[10];
	WORD Result;
        
	Result = FALSE;
	DAR = DAR_WrongType;

	if( (pData[0] == Structure_698) &&  (pData[1] == 0x03))//判断属性
	{
		if( pData[2] ==Long_unsigned_698)//冻结周期
		{
			lib_ExchangeData(Buf,&pData[3],2);//取CYCLE
			if( pData[5] == OAD_698 )
			{
				memcpy(Buf+2,&pData[6],4);//取OAD	
				if( pData[10] == Long_unsigned_698 )
				{
					lib_ExchangeData(Buf+6,&pData[11],2);//取CYCLE
					DAR = DAR_Success;
				}
			}
		}
	}
	
	if( DAR ==DAR_Success )
	{
		Result = api_AddFreezeAttribue( Type, Buf, 1 );
		if( Result == TRUE )
		{
			DAR = DAR_Success;
		}
		else
		{
			DAR = DAR_RefuseOp;
		}
	}
        
    return DAR;
}

//--------------------------------------------------
//功能描述:    698批量添加关联对象属性
//         
//参数:      BYTE Type[in]    数据OI
//         
//         BYTE *pData[in]  数据指针
//                 
//返回值:     BYTE DAR结果
//         
//备注内容:    无
//--------------------------------------------------
BYTE ActionAddFreezeList( WORD Type,BYTE *pData )
{
	BYTE DAR,Buf[MAX_FREEZE_ATTRIBUTE*8+30],i;		//最大的可设置的关联对象属性个数是54个，应留出足够的空间避免存在超内存现象--ml
    WORD Result;
        
	Result = FALSE;
	DAR = DAR_WrongType;

	if( pData[0] == Array_698 )
	{
	    if( pData[1] <= MAX_FREEZE_ATTRIBUTE )//不能超过最大关联对象个数
	    {
            for( i=0; i< pData[1]; i++ )
            {
                if( pData[2+13*i] == Structure_698 )
                {
                    if( pData[3+13*i] == 3 )
                    {
                        lib_ExchangeData( &Buf[8*i], &pData[3+2+13*i],2);//冻结周期
                        memcpy( &Buf[2+8*i], &pData[3+2+2+1+13*i],4);//OAD
                        lib_ExchangeData( &Buf[6+8*i], &pData[3+2+2+1+4+1+13*i],2);//冻结深度
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    break;
                }
            }

            if( i == pData[1] )//没有错误
            {
                DAR = DAR_Success;
            }
	    }
	    else
	    {
            DAR = DAR_OverRegion;
	    }
	}

	if( DAR == DAR_Success)
	{
		Result = api_AddFreezeAttribue( Type, Buf, pData[1]);
        if( Result == FALSE )
        {
            DAR = DAR_RefuseOp;
        }
        else
        {
            DAR = DAR_Success;
        }
	}
        
    return DAR;
}

//--------------------------------------------------
//功能描述:    698操作冻结请求
//         
//参数:      BYTE Ch[in]          通道选择
//         
//         BYTE *pOMD[in]       方法指针
//         
//返回值:     eAPPBufResultChoice  返回添加Buf结果     
//         
//备注内容:    无
//--------------------------------------------------
eAPPBufResultChoice ActionRequestFreeze( BYTE Ch, BYTE *pOMD)
{
	TFourByteUnion OAD;
	TTwoByteUnion OI;
	BYTE DAR,MethodNo,FreezeIndex;
	BYTE *pData;
	eAPPBufResultChoice eResultChoice;
	WORD Delay,Result;
	
	lib_ExchangeData(OI.b,pOMD,2);//取oi
	MethodNo = pOMD[2];//取方法
	pData = &pOMD[4];//取data
	DAR = DAR_WrongType;//默认失败
	//Type = OI.w  -0x5000;
	
	switch ( MethodNo )
	{
		case  1://复位
			DAR =DAR_RefuseOp;
			break;
		case  2://执行		
			break;
		case  3://触发冻结
		    if( OI.w != 0x5000 )//不是瞬时冻结
		    {
                DAR = DAR_RefuseOp;//拒绝操作
		    }
		    else
		    {
    			if( pData[0] == Long_unsigned_698 )
    			{
    				lib_ExchangeData( (BYTE*)&Delay, &pData[1], 2);
					FreezeIndex = api_GetFreezeIndex( OI.w );
					Result = api_SetFreezeFlag( (eFreezeType)FreezeIndex, Delay );
    				if( Result == TRUE )//返回结果正确
    				{
    					DAR = DAR_Success;
    				}
    				else
    				{
    					DAR = DAR_HardWare;//为固定返回不可随意更改
    				}
    			}	
		    }
	
			break;
		case  4://添加一个冻结对象属性
			DAR = ActionAddFreeze( OI.w, pData);
			break;
		case  5://删除一个冻结对象属性
			if( pData[0] == OAD_698 )
			{
				memcpy(OAD.b,&pData[1],4);//取OAD	
				Result = api_DeleteFreezeAttribue( OI.w, eDELETE_ATT_BY_OAD,OAD.b );
				if( Result == TRUE )
				{
					DAR = DAR_Success;
				}
				else
				{
					DAR = DAR_HardWare;
				}
			}	
			break;
		case  7://批量添加冻结对象属性
            DAR = ActionAddFreezeList( OI.w, pData );
			break;		
		case  8://删除整个冻结对象属性
		    if( pData[0] == NULL_698 )
		    {
                Result = api_DeleteFreezeAttribue( OI.w, eDELETE_ATT_ALL,OAD.b );
                if( Result == TRUE )
                {
                    DAR = DAR_Success;
                }
                else
                {
                    DAR = DAR_HardWare;
                }
		    }

			break;
		default:
			break;
	}
	
	eResultChoice = DLT698AddOneBuf(Ch,0x00, DAR);//添加DAR
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	
	eResultChoice = DLT698AddOneBuf(Ch,0x55, 00);//OPTIONAL NO CHOICE DATA 
	
	return eResultChoice;	
}
//--------------------------------------------------
//功能描述:    ESAM数据回抄
//         
//参数:      BYTE Ch[in]          通道选择
//         
//         BYTE *pData[in]      数据指针
//         
//返回值:     eAPPBufResultChoice  返回添加BUF结果
//         
//备注内容:    无
//--------------------------------------------------
eAPPBufResultChoice ActionEsamDataRead( BYTE Ch,  BYTE *pData)//数据回抄
{
	WORD Result;
	BYTE Buf[10];
	TTwoByteUnion Len,Leninpage;
	eAPPBufResultChoice eResultChoice;
		
	if( pData[0] == SID_698 )
	{
		Deal_SIDDate(0x00,&pData[1], &pData[1],ProtocolBuf);
		Result = api_AuthDataSID( ProtocolBuf );
		if( Result == TRUE )
		{
			Len.b[0] = ProtocolBuf[1];//获取长度
			Len.b[1] = ProtocolBuf[0];
			
			Leninpage.w = Deal_698DataLenth( NULL, Len.b, eUNION_OFFSET_TYPE );
			memmove( &ProtocolBuf[3+Leninpage.w],&ProtocolBuf[2],Len.w);
			ProtocolBuf[0] = DAR_Success;
			ProtocolBuf[1] = 0x01;
			ProtocolBuf[2] = Octet_string_698;
			Leninpage.w = Deal_698DataLenth( &ProtocolBuf[3], Len.b, eUNION_TYPE );
           // memcpy( &Buf[3+Leninpage.w], &ProtocolBuf[2],Len.w );
			eResultChoice = DLT698AddBuf(Ch, 0x55,ProtocolBuf ,(3+Leninpage.w+Len.w));
		}
		else
		{
			Buf[0] = DAR_EsamFial;//AR 成功	
			Buf[1] = 0;
			eResultChoice = DLT698AddBuf(Ch, 0x55,Buf ,2);
		}

	}
	else
	{
		Buf[0] = DAR_WrongType;//AR 成功	
		Buf[1] = 0;
		eResultChoice = DLT698AddBuf(Ch, 0x55,Buf ,2);
	}

	return eResultChoice;
}
//--------------------------------------------------
//功能描述:    ESAM数据更新
//         
//参数:      BYTE *pData[in]      数据指针         
//         
//返回值:     BYTE DAR结果
//         
//备注内容:    无
//--------------------------------------------------
BYTE ActionEsamDataUpdate( BYTE *pData)//数据更新
{
	WORD Result,OI;
	BYTE i,DAR,Type,OAD[4];
	TTwoByteUnion Len,Leninpage;
	
	Result = FALSE;
	Type = 0;
	
	if( pData[0] == Structure_698 )//结构体
	{
		if( pData[2] == Octet_string_698)//oi
		{		    
			Leninpage.w = Deal_698DataLenth( &pData[3], Len.b, ePROTOCOL_TYPE);//8是类型
			lib_ExchangeData((BYTE *)&OI, &pData[3+Leninpage.w], 2);//获取oi
			memcpy( OAD, &pData[3+Leninpage.w],4 );
			
			if( OI == 0x4002)//表号-- 私钥下不允许更新表号
			{
				if( api_GetRunHardFlag(eRUN_HARD_COMMON_KEY) == FALSE )
			    {
	                return DAR_PassWord;
			    }
			    
				Type = 0;
			}
			else
			{
				Type = 0x55;
			}
			
			Deal_SIDMACDate(Type, &pData[3+Leninpage.w+Len.w+1], &pData[3],ProtocolBuf);
            if( OI == 0x401B)//备用套阶梯
			{
				#if( PREPAY_MODE == PREPAY_LOCAL )
				for( i=0; i<4; i++ ) 
                {	
                    lib_InverseData(ProtocolBuf+9+52+3*i,3);//年结算日字节交换顺序
                }
                if( (lib_IsAllAssignNum( ProtocolBuf+9, 0x00,24 )==FALSE) 
                    && (api_JudgeYearClock( ProtocolBuf+9+52 ) == FALSE) ) 
                {	
                    return DAR_WrongType; //阶梯值非全0但是4个阶梯结算日全部无效则不允许设置
                }
                for( i=0; i<4; i++ ) 
                {	
                    lib_InverseData(ProtocolBuf+9+52+3*i,3);//年结算日字节交换顺序
                }
                #else
				return DAR_NoObject;
                #endif
			}
			//不进行esam验证，将验证放到了判断格式后面进行，此处如果是密文可能回有问题，与复旦微确认也是如此---jwh
			DAR = EsamDataUpdate( OAD, (BYTE*)&pData[3+Leninpage.w+4]);
			
		}
		else
		{
			DAR = DAR_WrongType;//AR 硬件时效	
		}

	}
	else
	{
		DAR = DAR_WrongType;//AR 硬件时效	
	}

	return DAR;
}
#if( PREPAY_MODE == PREPAY_LOCAL)
//--------------------------------------------------
//功能描述:    钱包操作esam验证
//         
//参数:     WORD Money[in]         充值金额
//         
//        WORD BuyTimes[in]      购电次数
//         
//        BYTE *pCustomCode[in]  客户编号
//
//        BYTE *pSIDMAC[in]      验证指针
//
//返回值:    WORD 操作结果
//         
//备注内容:    无
//--------------------------------------------------
WORD api_DealMoneyDataToESAM( DWORD Money, DWORD BuyTimes, BYTE *pCustomCode, BYTE *pSIDMAC )
{
    BYTE Buf[100];
    BYTE SMOffest,SMOffest1;
    TTwoByteUnion SMLen,SMLen1;
    WORD Result;
    
    memcpy( Buf, pSIDMAC, 4);//组前4个字节
    SMOffest = Deal_698DataLenth( pSIDMAC+4, SMLen.b, ePROTOCOL_TYPE);
    if( SMLen.w > ( sizeof(Buf) - 4 ) )//防止数组越界
    {
    	return FALSE;
    }
    memcpy( (BYTE*)&Buf[4], pSIDMAC+4+SMOffest, SMLen.w);//组后面的数据
    
    lib_ExchangeData( (BYTE*)&Buf[4+SMLen.w], (BYTE*)&Money, 4 );//组4字节金额
    lib_ExchangeData( (BYTE*)&Buf[4+SMLen.w+4], (BYTE*)&BuyTimes, 4 );//组4字节金额
    memcpy( (BYTE*)&Buf[4+SMLen.w+4+4], pCustomCode, 6 );//组4字节金额
    
    SMOffest1 = Deal_698DataLenth( pSIDMAC+4+SMOffest+SMLen.w, SMLen1.b, ePROTOCOL_TYPE);
    memcpy( (BYTE*)&Buf[4+SMLen.w+4+4+6], pSIDMAC+4+SMOffest+SMLen.w+SMOffest1, SMLen1.w );//组帧MAC

    Result = api_AuthDataSID( Buf );
    if( Result == FALSE )
    {
        return FALSE;
    }

    return TRUE;
}

//--------------------------------------------------
//功能描述:    ESAM钱包操作
//         
//参数:      BYTE Ch[in]          通道选择
//         
//         BYTE *pData[in]      数据指针
//         
//返回值:     BYTE
//         
//备注内容:    无
//--------------------------------------------------
BYTE ActionEsamMoney(BYTE *pData)//钱包操作
{
    BYTE Type,Offest,Offest1;
    BYTE pCustomCode[16],pMeterNo[16],*pSIDMAC;
    TTwoByteUnion Len,Len1,Len2;
    DWORD Money,BuyTimes;
    WORD DAR;

    Len.w = 0;
    Len1.w = 0;
    Len2.w = 0;
    
	if( pData[0] == Structure_698)//结构体
	{
		if( pData[1] == 0x06 )//sequence of data
		{
			if( pData[2] == Integer_698 )//类型
			{
			    Type = pData[3];
				if( pData[4] == Double_long_unsigned_698 )//购电金额
				{
                    lib_ExchangeData( (BYTE *)&Money, (BYTE*)&pData[5], 4 );
                    if( pData[5+4] == Double_long_unsigned_698 )
                    {
                        lib_ExchangeData( (BYTE*)&BuyTimes, (BYTE*)&pData[5+4+1], 4 );//购电次数
                        if( pData[5+4+1+4] == Octet_string_698 )
                        {
                            Offest = Deal_698DataLenth( (BYTE*)&pData[5+4+1+4+1], Len.b, ePROTOCOL_TYPE);
                            if( Len.w != 6 )
                            {
                                return DAR_OverRegion;
                            }
                            
                            memcpy( pCustomCode, (BYTE*)&pData[5+4+1+4+1+Offest], Len.w );//获取表号

                            if( pData[5+4+1+4+1+Offest+Len.w] == SID_MAC_698 )
                            {
                                Len1.w = GetProtocolDataLen( (BYTE*)&pData[5+4+1+4+1+Offest+Len.w] );
                                pSIDMAC = (BYTE*)&pData[5+4+1+4+1+Offest+Len.w+1];
                                if( pData[5+4+1+4+1+Offest+Len.w+Len1.w] == Octet_string_698 )
                                {
                                    Offest1 = Deal_698DataLenth( (BYTE*)&pData[15+Offest+Len.w+Len1.w+1], Len2.b, ePROTOCOL_TYPE);
                                    if( Len2.w != 6 )
                                    {
                                        return DAR_OverRegion;
                                    }

                                    memcpy( pMeterNo, (BYTE*)&pData[15+Offest+Len.w+Len1.w+1+Offest1],Len2.w );
                                    DAR = api_RemoteActionMoneybag( Type, Money, BuyTimes, pCustomCode, pSIDMAC, pMeterNo);
                                    return DAR;
                                }
                            }
                        }
                    }
				}
				
			}
		}
	}
    return FALSE;
}
//--------------------------------------------------
//功能描述:    ESAM初始化操作
//         
//参数:      BYTE Ch[in]          通道选择
//         
//         BYTE *pData[in]      数据指针
//         
//返回值:     BYTE
//         
//备注内容:    无
//--------------------------------------------------
BYTE ActionEsamIntMoney( BYTE Ch, BYTE *pData)//钱包操作
{
    DWORD Money;
    WORD Result,SMOffest,SMOffest1,wTmpClearMeterType;
    TTwoByteUnion SMLen,SMLen1,OI;
    BYTE Buf[100];
    
    OI.w = 0XF100;
    
    if( api_GetRunHardFlag( eRUN_HARD_COMMON_KEY ) != TRUE )//私钥不允许钱包初始化
    {
        return DAR_PassWord;
    }
    
    if( pData[0] == Structure_698 )
    {
        if( pData[1] == 2 )
        {
            if( pData[2] == Double_long_unsigned_698 )
            {
                lib_ExchangeData( (BYTE*)&Money, (BYTE*)&pData[3], 4 );//获取剩余金额
                if( pData[3+4] == SID_MAC_698 )
                {
                    memcpy( Buf, (BYTE*)&pData[3+4+1], 4);//组前4个字节
                    SMOffest = Deal_698DataLenth( (BYTE*)&pData[3+4+1+4], SMLen.b, ePROTOCOL_TYPE);
                    if( SMLen.w > (sizeof(Buf) - 4) )//防止数组越界
                    {
                    	return DAR_OverRegion;
                    }
                    memcpy( (BYTE*)&Buf[4], &pData[3+4+1+4+SMOffest], SMLen.w);//组后面的数据
                    memcpy( (BYTE*)&Buf[4+SMLen.w], (BYTE*)&pData[3], 4 );//组4字节金额
                    SMOffest1 = Deal_698DataLenth( (BYTE*)&pData[3+4+1+4+SMOffest+SMLen.w], SMLen1.b, ePROTOCOL_TYPE);
                    if( SMLen1.w > ( sizeof(Buf)-4-SMLen.w-4 ) )//防止数组越界
                    {
                    	return DAR_OverRegion;
                    }
                    memcpy( (BYTE*)&Buf[4+SMLen.w+4], &pData[3+4+1+4+SMOffest+SMLen.w+SMOffest1], SMLen1.w );//组帧MAC

                    Result = api_AuthDataSID( Buf );
                    if( Result == FALSE )
                    {
                        return DAR_Symmetry;
                    }
                    
                    api_UpdataRemainMoney( eInitMoneyMode, Money, 0 );

                    //wTmpClearMeterType = 0x1111;eCLEAR_FACTORY
                    //api_WritePara(0, GET_STRUCT_ADDR(TFPara1, wClearMeterType), sizeof(WORD), (BYTE *)&wTmpClearMeterType );
                    api_SetClearFlag( eCLEAR_MONEY, 0 ); //设置清零标志,698.45置清零标志统一用 api_SetClearFlag wlk
                    api_WriteFreeEvent(EVENT_DATA_INIT_698,OI.w);
					api_SavePrgOperationRecord( ePRG_CLEAR_METER_NO );
                    g_byClrTimer_500ms = 2;//默认延时1秒，这样实际延时范围500~1500ms,1200bps 连续发送20字节时间为 183ms
                }
            }
        }
    }

    return DAR_Success;
}
#endif
//--------------------------------------------------
//功能描述:    ESAM密钥更新
//         
//参数:      BYTE *pData[in]      数据指针
//                  
//返回值:     eAPPBufResultChoice  返回添加BUF结果  
//         
//备注内容:    无
//--------------------------------------------------
BYTE ActionUpdateEsamKey( BYTE *pData )//密钥更新
{	
	WORD Result;
	BYTE DAR;
	TTwoByteUnion Len,Leninpage;
	TFourByteUnion OAD;
	
	DAR = DAR_WrongType;
	OAD.d = 0x000400F1;
	
	if( pData[0] == Structure_698 )//结构体
	{
		if( pData[1] == 0x02 )//sequence of data
		{
			if( pData[2] == Octet_string_698)
			{
			    api_WritePreProgramData( 0,OAD.d );
				Leninpage.w = Deal_698DataLenth( &pData[3], Len.b, ePROTOCOL_TYPE);
				Deal_SIDMACDate(0x00, &pData[3+Leninpage.w+Len.w+1],&pData[3],ProtocolBuf);
				Result = api_AuthDataSID( ProtocolBuf );
				if( Result == TRUE )
				{
					DAR = DAR_Success;//AR 成功
					api_SavePrgOperationRecord( ePRG_UPDATE_KEY_NO ); //密钥更新事件
					api_FlashKeyStatus();
				}
				else
				{
					DAR = DAR_EsamFial;//AR 硬件时效	
				}			
			}		
		}
		else
		{
			DAR = DAR_WrongType;//AR 硬件时效	
		}
	}
	else
	{
		DAR = DAR_WrongType;//AR 硬件时效	
	}
	
	return DAR;
}
//--------------------------------------------------
//功能描述:    设置ESAM时效门限
//         
//参数:      BYTE *pData[in]      数据指针
//                  
//返回值:     BYTE DAR结果
//         
//备注内容:    无
//--------------------------------------------------
BYTE ActionEsamConsultationTime( BYTE *pData )//设置时效门限
{
	WORD Result;
	BYTE DAR;
	TTwoByteUnion Len,Leninpage;
	
	DAR = DAR_WrongType;
	
	if( pData[0] == Structure_698 )//结构体
	{
		if( pData[1] == 0x02 )//sequence of data
		{
			if( pData[2] == Octet_string_698)
			{
				Leninpage.w = Deal_698DataLenth( &pData[3], Len.b, ePROTOCOL_TYPE);
				Deal_SIDDate(0x55,&pData[3+Leninpage.w+Len.w+1],&pData[3],ProtocolBuf);
				Result = api_AuthDataSID( ProtocolBuf );
				if( Result == TRUE )
				{
					DAR = DAR_Success;//AR 成功	
				}
				else
				{
					DAR = DAR_HardWare;//AR 硬件时效	
				}
			
			}
		}
		else
		{
			DAR = DAR_WrongType;//AR 硬件时效	
		}
	}
	else
	{
		DAR = DAR_WrongType;//AR 硬件时效	
    }

	return DAR;
}
//--------------------------------------------------
//功能描述:    操作ESAM协商失效
//         
//参数:      BYTE Ch[in]      通道选择
//      
//         BYTE *pData[in]  数据指针
//         
//返回值:     eAPPBufResultChoice  返回添加BUF结果 
//         
//备注内容:    无
//--------------------------------------------------
eAPPBufResultChoice ActionEsamDisableConsultation( BYTE Ch, BYTE *pData)//协商失效
{
	BYTE Buf[20];
	eAPPBufResultChoice eResultChoice;
	
	if( pData[0] == NULL_698 )//数据为Null
    {		
		if( CURR_COMM_TYPE(Ch) == COMM_TYPE_TERMINAL )
		{
			api_ClrSysStatus( eSYS_STATUS_ID_698TERMINAL_AUTH );
			APPConnect.ConnectInfo[eConnectTerminal].ConnectValidTime = 1;	//1秒后关闭esam
			APPConnect.ConnectInfo[eConnectTerminal].ConstConnectValidTime = 1;
		}
		else
		{
			api_ClrSysStatus( eSYS_STATUS_ID_698MASTER_AUTH );
			APPConnect.ConnectInfo[eConnectMaster].ConnectValidTime = 1;	//1秒后关闭esam
			APPConnect.ConnectInfo[eConnectMaster].ConstConnectValidTime = 1;
		}

		api_ClrSysStatus( eSYS_STATUS_ID_698GENERAL_AUTH );
      
        Buf[0] = DAR_Success;//AR 成功    
        Buf[1] = 0x01;//choice data
        Buf[2] = DateTime_S_698;//返回当前时间
        api_GetRtcDateTime( DATETIME_20YYMMDDhhmmss,&Buf[3]);
        lib_InverseData( &Buf[3],  2 );
        eResultChoice = DLT698AddBuf(Ch, 0x55,Buf ,10);
    }
    else
    {
        eResultChoice = DLT698AddOneBuf( Ch, 0x55, DAR_WrongType );//添加错误
        if( eResultChoice != eADD_OK )
    	{
    		return eResultChoice;
    	}
        eResultChoice = DLT698AddOneBuf(Ch,0x55, 00);//OPTIONAL NO CHOICE DATA 
    }

	return eResultChoice;
}

//--------------------------------------------------
//功能描述: 红外请求
//         
//参数:      BYTE Ch[in]          通道选择
//         
//         BYTE *pData[in]      数据指针
//         
//返回值:     eAPPBufResultChoice  添加buf结果
//         
//备注内容:    无
//--------------------------------------------------
eAPPBufResultChoice ActionIRRequest( BYTE Ch, BYTE *pData)//设置时效门限
{
	WORD Result;
	BYTE DAR;
	BYTE Tmp_RN[MAX_RN_SIZE+3],OutBuf[15];
	eAPPBufResultChoice eResultChoice;

	DAR = DAR_WrongType;
	//断开已建立的应用连接
	if( api_GetSysStatus( eSYS_STATUS_POWER_ON ) == FALSE )//低功耗不可执行
	{
		DAR = DAR_PassWord;
	}
	else
	{	
		DAR = DAR_Success;
		
		//if( (Ch == eIR)&&(CURR_COMM_TYPE(Ch) != COMM_TYPE_TERMINAL) )
		//{
		//	//物理通道需是IR通道，客户机地址必须是主站
		//	api_ClrSysStatus( eSYS_IR_ALLOW_PRG ); //清红外认证标志
		//}
		//else if( (Ch != eIR)&&(CURR_COMM_TYPE(Ch) == COMM_TYPE_TERMINAL) )
		if(CURR_COMM_TYPE(Ch) == COMM_TYPE_TERMINAL)
		{
			api_ClrSysStatus( eSYS_TERMINAL_ALLOW_PRG );//清终端身份认证标志
		}
		else
		{
			DAR = DAR_RefuseOp;
		}

		if( DAR == DAR_Success )
		{
			//api_ResetEsamSpi( );//复位ESAM
			APPConnect.ConnectInfo[eConnectGeneral].ConnectValidTime = 1024;//打开esam 1024秒
			APPConnect.ConnectInfo[eConnectGeneral].ConstConnectValidTime = 1024;//打开esam 1024秒
			
            if( pData[0] == RN_698 )//随机数
            {
               if( pData[1] < MAX_RN_SIZE )
               {
                    Tmp_RN[0] = pData[1];
                    memcpy( (BYTE*)&Tmp_RN[1], (BYTE*)&pData[2], pData[1] );//获取随机数
                    DAR = DAR_Success;
               }
            }
            else
            {
                DAR = DAR_WrongType;//AR 硬件时效   
            }
            
            if( DAR == DAR_Success )
            {
				//if( Ch == eIR )
				//{
				//	Result = api_EsamIRRequest( Tmp_RN, OutBuf ); //红外查询 获取8字节随机数1密文
				//}
				//else
				{
					Result = api_EsamTerminalRequest( Tmp_RN, OutBuf ); //终端身份认证 获取8字节随机数1密文
				}
				if( Result == FALSE )
				{
					DAR = DAR_EsamFial;
				}
				else
				{
				    DAR = DAR_Success;
				}

				api_ReadEsamRandom( 8, Tmp_RN );
				memmove( Tmp_RN, Tmp_RN+2, 8);
			}
		}
		else
		{
			DAR = DAR_RefuseOp;   
		}
	}

	if( DAR == DAR_Success )
	{
		ProtocolBuf[0] = DAR_Success;//AR 成功	
		ProtocolBuf[1] = 0x01;//choice data
		ProtocolBuf[2] = Structure_698;//no data
		
		ProtocolBuf[3] = 04;

		ProtocolBuf[4] = Octet_string_698;//表号
		ProtocolBuf[5] = 6;
		api_ReadEsamData(0x0001, 0x0007, 0x0006, ProtocolBuf+6);//获取表号(6个字节) 上面easm通信成功，认为esam没问题

		ProtocolBuf[12] = Octet_string_698;//ESAM序列号
		ProtocolBuf[13] = 8;
		api_GetEsamInfo( 2, ProtocolBuf+14 );//8个字节 上面easm通信成功，认为esam没问题

		ProtocolBuf[22] = Octet_string_698;//随机数1密文
		ProtocolBuf[23] = 8;
		memcpy(ProtocolBuf+24, OutBuf, 8);

		ProtocolBuf[32] = RN_698;//随机数2
		ProtocolBuf[33] = 8;
		memcpy( ProtocolBuf+34, Tmp_RN, 8);

		eResultChoice = DLT698AddBuf(Ch, 0x55,ProtocolBuf ,42 );
	}
	else
	{
		ProtocolBuf[0] = DAR;
		ProtocolBuf[1] = 0;
		eResultChoice = DLT698AddBuf(Ch, 0x55,ProtocolBuf ,2);
	}

    return eResultChoice;
}
//--------------------------------------------------
//功能描述: 红外认证
//         
//参数:      BYTE Ch[in]          通道选择
//         
//         BYTE *pData[in]      数据指针
//         
//返回值:     byte 操作结果DAR
//         
//备注内容:    无
//--------------------------------------------------
BYTE ActionIRAuth( BYTE Ch, BYTE *pData)//设置时效门限
{
    BYTE DAR,Result;
    DWORD tdw;

    DAR = DAR_WrongType;
    
    if(  api_GetSysStatus(eSYS_STATUS_POWER_ON) != FALSE )//仅支持红外通道 低功耗不可执行
    {
        if( pData[0] == Octet_string_698 )//获取随机数2密文 
        {
            memcpy( ProtocolBuf, (BYTE*)&pData[2], pData[1] );
            Result = api_EsamIRAuth( ProtocolBuf );
            if( Result == FALSE )
            {
                DAR = DAR_EsamFial;
            }
            else
            {
				DAR = DAR_Success;
				//if( (Ch == eIR)&&(CURR_COMM_TYPE(Ch) != COMM_TYPE_TERMINAL) )
				//{
				//	api_ReadPrePayPara( eIRTime, (BYTE*)&tdw );
				//	APPConnect.IRAuthTime = (tdw*60);//将时效分钟改为秒
				//	api_SetSysStatus( eSYS_IR_ALLOW_PRG );//置位红外编程允许
				//	
				//	APPConnect.ConnectInfo[eConnectGeneral].ConnectValidTime = (APPConnect.IRAuthTime+2);//认证期间不进行esam复位，一般密码的时间比红外时间大2秒
				//	APPConnect.ConnectInfo[eConnectGeneral].ConstConnectValidTime = (APPConnect.IRAuthTime+2);

				//	//断开主站应用链接
				//	APPConnect.ConnectInfo[eConnectMaster].ConnectValidTime = 1;
				//	APPConnect.ConnectInfo[eConnectMaster].ConstConnectValidTime = 1;//1s后关闭终端协商
				//	api_ClrSysStatus( eSYS_STATUS_ID_698MASTER_AUTH );
				//}
				//else if( (Ch != eIR)&&(CURR_COMM_TYPE(Ch) == COMM_TYPE_TERMINAL) )
				if(CURR_COMM_TYPE(Ch) == COMM_TYPE_TERMINAL)
				{
					api_ReadPrePayPara( eIRTime, (BYTE*)&tdw );
					APPConnect.TerminalAuthTime = (tdw*60);		//将时效分钟改为秒
					api_SetSysStatus( eSYS_TERMINAL_ALLOW_PRG );//置位终端身份认证成功标志

					APPConnect.ConnectInfo[eConnectGeneral].ConnectValidTime = (APPConnect.TerminalAuthTime+2);//认证期间不进行esam复位，一般密码的时间比红外时间大2秒
					APPConnect.ConnectInfo[eConnectGeneral].ConstConnectValidTime = (APPConnect.TerminalAuthTime+2);

					//断开终端应用链接
					APPConnect.ConnectInfo[eConnectTerminal].ConnectValidTime = 1;		//参考 协商失效ESAM接口类 方法5
					APPConnect.ConnectInfo[eConnectTerminal].ConstConnectValidTime = 1;
					api_ClrSysStatus(eSYS_STATUS_ID_698TERMINAL_AUTH);
					
				}
				else
				{
					DAR = DAR_RefuseOp;
				}
            }
        }
    }
    else
    {
        DAR = DAR_RefuseOp;
    }

    return DAR;
}

//-----------------------------------------------
//功能描述  :    操作ESAM
//参数:  
//           BYTE Ch[in]          通道选择   
//
//           BYTE *pOMD[in]       方法指针
//
//返回值:       eAPPBufResultChoice  返回添加Buf结果
//
//备注内容  :    有数据返回的直接return
//-----------------------------------------------
eAPPBufResultChoice ActionRequestEsam( BYTE Ch,  BYTE *pOMD)
{
	BYTE DAR;
	BYTE *pData;
	eAPPBufResultChoice eResultChoice;
	
	pData = &pOMD[4];
	DAR = DAR_WrongType;//默认失败
	
	switch( pOMD[2] )
	{
	case 0x03://数据回抄
		eResultChoice = ActionEsamDataRead( Ch, pData );
		return eResultChoice;
		break;
	case 0x04://数据更新
		DAR = ActionEsamDataUpdate( pData );
		break;
	case 0x05://协商失效
		eResultChoice = ActionEsamDisableConsultation( Ch, pData );
		return eResultChoice;
		break;
	#if( PREPAY_MODE == PREPAY_LOCAL)
	case 0x06://钱包操作---未做
		DAR = ActionEsamMoney( pData );
		break;
	#endif
	case 0x07://密钥更新
		DAR = ActionUpdateEsamKey( pData );
		break;
	case 0x08://证书更新		
		break;
	case 0x09://设置ESAM 参数
		DAR = ActionEsamConsultationTime( pData );
		if( DAR == DAR_Success )
		{
			api_FlashIdentAuthFlag();   //刷新身份认证权限标志
		}
		break;
	#if( PREPAY_MODE == PREPAY_LOCAL)	
	case 0x0A://钱包初始化
	    DAR = ActionEsamIntMoney( Ch, pData );
		break;	
	#endif
	case 0x0B://红外认证请求
	    eResultChoice = ActionIRRequest( Ch, pData );
	    return eResultChoice;
		break; 
	case 0x0C://红外认证指令
	    DAR = ActionIRAuth(Ch, pData );
		break;
	default:		
		break;
	}
	
	eResultChoice = DLT698AddOneBuf(Ch,0x00, DAR);//添加DAR
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	eResultChoice = DLT698AddOneBuf(Ch,0x55, 00);//OPTIONAL NO CHOICE DATA 
	
    return eResultChoice;
}
//--------------------------------------------------
//功能描述:    广播校时
//         
//参数:      BYTE Ch[in]          通道选择
//         
//         BYTE *pOMD[in]       方法指针
//         
//返回值:     eAPPBufResultChoice  返回添加Buf结果      
//         
//备注内容:    无
//--------------------------------------------------
eAPPBufResultChoice ActionRequestTimeBroadCast( BYTE Ch,  BYTE *pOMD)
{
	BYTE DAR;
	eAPPBufResultChoice eResultChoice;
	DWORD AbsMeterSecTime,AbsSetSecTime,AbsZeroHourSecTime;
	BYTE *pData;
	TRealTimer TmpRealTimer;
	TTimeBroadCastPara tmpTimeBroadCastPara;

	pData = &pOMD[4];//强制进行数据返回,先默认这样
    DAR = DAR_WrongType;

	switch( pOMD[2] )//Method )
	{
		case 127://广播校时
			if( pData[0] != DateTime_S_698 )
			{
				break;
			}
			
			if( (api_GetRunHardFlag( eRUN_HARD_ALREADY_BROADCAST_FLAG ) == TRUE)
			&&(APPData[Ch].eSafeMode == eNO_SECURITY) )//明文广播校时一天一次
			{
			    DAR = DAR_TempFail;
				break;
			}
			
			api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss, (BYTE *)&TmpRealTimer );//获取当前时间
			AbsMeterSecTime = api_CalcInTimeRelativeSec((TRealTimer*)&TmpRealTimer);//计算当前时间绝对秒数

			TmpRealTimer.Hour = 0;//获取零点数据
			TmpRealTimer.Min = 0;
			TmpRealTimer.Sec = 0;
			AbsZeroHourSecTime = api_CalcInTimeRelativeSec((TRealTimer*)&TmpRealTimer);//计算当前时间绝对秒数
			
			lib_InverseData(&pData[1], 2);
			memcpy((BYTE*)&(TmpRealTimer.wYear),&pData[1],sizeof( TRealTimer ));//获取广播校时时间
			AbsSetSecTime = api_CalcInTimeRelativeSec((TRealTimer*)&TmpRealTimer);//计算当前时间绝对秒数
			//对下发的时间进行合法值判断，不合法的时间不允许校时
			if(AbsSetSecTime == 0xffffffff)
			{
				DAR = DAR_OverRegion;
				break;
			}
			if( (api_GetRunHardFlag(eRUN_HARD_ERR_RTC_FLAG)==FALSE) && (api_GetRunHardFlag(eRUN_HARD_CLOCK_BAT_LOW)==FALSE) )	//时钟正常和电池正常进行判断
			{
				//获取广播校时参数
				if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR( ProSafeRom.TimeBroadCastPara), sizeof(TTimeBroadCastPara), (BYTE*)&tmpTimeBroadCastPara ) == FALSE )
				{
					tmpTimeBroadCastPara.TimeMinLimit = TimeBroadCastTimeMinLimit; //附默认值
					tmpTimeBroadCastPara.TimeMaxLimit = TimeBroadCastTimeMaxLimit; 
				}
				
				if( APPData[Ch].eSafeMode == eSECRET_MAC )
				{
					//小于广播校时限值 不予校时
					if( labs(AbsMeterSecTime-AbsSetSecTime) < tmpTimeBroadCastPara.TimeMinLimit )
					{
					    DAR = DAR_OverRegion;
						break;
					}
				}
				else if( APPData[Ch].eSafeMode == eNO_SECURITY )//明文模式
				{
					//小于校时最小时间限制不能校时
					if( labs(AbsMeterSecTime-AbsSetSecTime) < tmpTimeBroadCastPara.TimeMinLimit )
					{
						DAR = DAR_OverRegion;
						break;
					}
					//小于-等于最大时间限制可以校时
					if( labs(AbsMeterSecTime-AbsSetSecTime) > tmpTimeBroadCastPara.TimeMaxLimit )
					{
					    if( api_GetRunHardFlag( eRUN_HARD_ALREADY_PLAINTEXT_BROADCAST_ERR_FLAG ) == FALSE )
						{
							//置RTC错误标志
							api_SetRunHardFlag(eRUN_HARD_BROADCAST_ERR_FLAG);
							//置明文广播校时故障错误标志
							api_SetRunHardFlag( eRUN_HARD_ALREADY_PLAINTEXT_BROADCAST_ERR_FLAG );
						}
						DAR = DAR_OverRegion;
						break;
					}
					//明文广播校时不允许跨天
					if ((AbsSetSecTime<AbsZeroHourSecTime)||(AbsSetSecTime>=(AbsZeroHourSecTime+24*60*60)))
					{
						DAR = DAR_OverRegion;
						break;
					}
					//不能跨结算日校时
					if( JudgeBroadcastTime( AbsSetSecTime ) == FALSE )
					{
					    DAR = DAR_OverRegion;
						break;
					}
					
				}
				else
				{
					DAR = DAR_RefuseOp;
					break;
				}
				api_WritePreProgramData( 0, 0x00020040 );//时间
				if( api_WriteMeterTime(&TmpRealTimer) == TRUE )
				{
					#if( SEL_DEMAND_2022 == NO )
				    #if( MAX_PHASE != 1)		
            		api_InitDemand( );//设置成功后，重新进行需量的计算
            		#endif
            		#endif
            		if( APPData[Ch].eSafeMode == eNO_SECURITY )//明文广播校时一天一次
            		{
						api_SetRunHardFlag( eRUN_HARD_ALREADY_BROADCAST_FLAG );
            		}
					api_SavePrgOperationRecord( ePRG_BROADJUST_TIME_NO );					
					//记录698广播校时自由事件
					api_WriteFreeEvent(EVENT_BROADCAST_WRITE_TIME, 0x0698);
					DAR = DAR_Success;
				}
			}	
			else
			{
				api_WritePreProgramData( 0, 0x00020040 );  //时间
				if( api_WriteMeterTime(&TmpRealTimer) == TRUE )
				{
					#if( SEL_DEMAND_2022 == NO )
				    #if( MAX_PHASE != 1)		
            		api_InitDemand( );//设置成功后，重新进行需量的计算
            		#endif
            		#endif
					//保存广播校时事件记录
					api_SavePrgOperationRecord( ePRG_BROADJUST_TIME_NO );
					if( APPData[Ch].eSafeMode == eNO_SECURITY )//明文广播校时一天一次
					{
						api_SetRunHardFlag( eRUN_HARD_ALREADY_BROADCAST_FLAG );
					}
					//记录698广播校时自由事件
					api_WriteFreeEvent(EVENT_BROADCAST_WRITE_TIME, 0x0698);
					DAR = DAR_Success;
				}
			}
		
			break;		
		default:
			break;
	}
	if( LinkData[Ch].AddressType == eBROADCAST_ADDRESS_MODE )//如果是广播地址，则不应答
    {
    	return eAPP_NO_RESPONSE;
    }
    else
    {
		if( DAR == DAR_Success )
		{
			eResultChoice = DLT698AddOneBuf(Ch,0x00, DAR_Success );//添加DAR
		}
		else
		{
			eResultChoice = DLT698AddOneBuf(Ch,0x00, DAR );//添加DAR
		}

		eResultChoice = DLT698AddOneBuf(Ch,0x55, 00);//OPTIONAL NO CHOICE DATA 
    	return eResultChoice;
    }	
}
//--------------------------------------------------
//功能描述:    操作端口
//         
//参数:      
//			Mode[in] 0:更新485	1：更新载波
//			BYTE 	*pData[in]      数据指针
//                  
//返回值:     	BYTE DAR结果
//         
//备注内容:    无
//--------------------------------------------------
BYTE ActionCOMRate( BYTE Mode, BYTE *pData )//操作端口波特率
{	
	TFourByteUnion OAD;
	BYTE TmpCommPara0,DAR;
	WORD Result;
	BYTE Channel;
	
	DAR = DAR_ChangeBaud;//DAR_WrongType;

	if( pData[0] == Structure_698 )
	{
		if( (pData[1] == 3) ||( pData[1] == 2 ))
		{
			if( pData[2] == OAD_698 )
			{
				lib_ExchangeData( OAD.b, &pData[3], 4);
				if( Mode == 0 )
				{
					if( (OAD.d == 0xf2010200) || (OAD.d == 0xf2010201) || (OAD.d == 0xf2010202))
					{
						if( OAD.d == 0xf2010202 )
						{
							Channel = 2;
							
						}
						else
						{
							Channel = 0;
						}
						
						if( pData[7] == COMDCB_698 )
						{
							if( (pData[8] < 11)&&(pData[8] != 5)&&(pData[8] > 1) )//bit0~bit2 表示波特率只能到7
							{
								//1200bps（2），2400bps（3），4800bps（4），7200bps（5），9600bps（6），19200bps（7），
								TmpCommPara0 = pData[8];//表示波特率
	
								if( pData[9] < 3)//校验位 
								{
									TmpCommPara0 |= (pData[9] <<  5);
	
									if( pData[10] == 8 )//只支持数据位为8
									{
										if( (pData[11] == 1) || (pData[11] == 2) )//支持停止位为1或2
										{
											if( pData[11] == 1 )
											{
												TmpCommPara0 &= (~BIT4);
											}
											else
											{
												TmpCommPara0 |= BIT4;
											}
											
											if( pData[12] < 3 )//流控
											{
												TmpCommPara0 |= (pData[12] << 7);
												Result = api_WritePara(1, (GET_STRUCT_ADDR(TFPara2, CommPara.I485)+Channel), 1, &TmpCommPara0);
												if( Result == FALSE )
												{
													DAR = DAR_HardWare;//此为固定返回，不可随意更改 与上层进行配合
												}
												else
												{
													if( Channel == eRS485_I )
													{
														Serial[eRS485_I].OperationFlag = 1;
													}
													else
													{
														//#if(MAX_COM_CHANNEL_NUM == 4 )//4个通道时支持485-2
														//Serial[eRS485_II].OperationFlag = 1;
														//#endif
													}
													
													DAR = DAR_Success;
												}
											}
										}
									}	
								}
							}
						}
					}
				}
				else
				{
					if(( OAD.d == 0xf20902FD ) || ( OAD.d == 0xf2090201 ))
					{
						if( pData[7] == COMDCB_698 )
						{
							if( (pData[8] < 11)&&(pData[8] != 5)&&(pData[8] > 1) )//bit0~bit2 表示波特率只能到10
							{
								
								TmpCommPara0 = (FPara2->CommPara.ComModule & 0xF0);//不赋值波特率
								//1200bps（2），2400bps（3），4800bps（4），7200bps（5），9600bps（6），19200bps（7），
								TmpCommPara0 |= (pData[8]&0x0F);//表示波特率
								if( TmpCommPara0 != FPara2->CommPara.ComModule )//协商参数变更
								{
									Result = api_WritePara(1, GET_STRUCT_ADDR(TFPara2, CommPara.ComModule), 1, &TmpCommPara0);
									if( Result == FALSE )
									{
										DAR = DAR_HardWare;//此为固定返回，不可随意更改 与上层进行配合
									}
									else
									{
										Serial[eCR].OperationFlag = 1;
										DAR = DAR_Success;
									}
								}
								else
								{
									DAR = DAR_Success;
								}
							}
						}
					}
				}
			}
		}
	}

	return DAR;
}
//--------------------------------------------------
//功能描述:    操作输入输出设备接口类
//         
//参数:      BYTE Ch[in]          通道选择
//         
//         BYTE *pOMD[in]       数据指针
//         
//返回值:     eAPPBufResultChoice  返回添加Buf结果   
//         
//备注内容:    无 
//--------------------------------------------------
eAPPBufResultChoice ActionRequestDevice( BYTE Ch,  BYTE *pOMD)
{	
	TTwoByteUnion OI;
	TFourByteUnion OAD;
	BYTE DAR,MethodNo,Mode,i;
	BYTE *pData,CanNode[1];
	WORD Result, Offset = 0;
	eAPPBufResultChoice eResultChoice;
	// TSafeMem_dev_work dev_work;

	lib_ExchangeData(OI.b,pOMD,2);//取oi
	MethodNo = pOMD[2];//取方法
	pData = &pOMD[4];//取data
	DAR = DAR_WrongType;//默认失败
	Result = FALSE;
	
	switch( OI.w )
	{
		case 0xf201://RS485	
		case 0xf209://载波
			if( OI.w == 0xf201 )
			{
				if( MethodNo != 127 )
				{
					break;
				}
				Mode = 0;
			}
			else
			{
				if(MethodNo != 128)
				{
					break;
				}
				Mode  = 1;
			}
			
			DAR = ActionCOMRate( Mode,pData );
			break;
		case 0xf205://继电器输出
			if( MethodNo != 127 )
			{
				break;
			}

			if( pData[0] == Structure_698 )
			{
				if( pData[1] == 2 )
				{
					if( pData[2] == OAD_698 )//继电器序号
					{
						lib_ExchangeData( OAD.b, &pData[3], 4);
						if( (OAD.d == 0xf2050200) || (OAD.d == 0xf2050201))
						{
							if( pData[7] == Enum_698 )//开关属性
							{
								if( pData[8] < 2 )
								{
									Result = WriteRelayPara( 3, &pData[8]);
									if( Result == FALSE )
									{
										DAR = DAR_HardWare;
									}
									else
									{
										DAR = DAR_Success;
									}
									
								}
							}
						}
					}
				}
			}
			break;
		case 0xf207://多功能端子
			if( MethodNo != 127 )
			{
				break;
			}

			if( pData[0] == Structure_698 )
			{
				if( pData[1] == 2 )
				{
					if( pData[2] == OAD_698 )//多功能端子
					{
						lib_ExchangeData( OAD.b, &pData[3], 4);
						if( (OAD.d == 0xf2070200) || (OAD.d == 0xf2070201))
						{
							if( pData[7] == Enum_698 )//工作模式
							{
							    // 多功能端子初始化
							    if( api_MultiFunPortSet(pData[8]))
							    {
								    DAR = DAR_Success;
							    }
							    else
							    {
								    DAR = DAR_RefuseOp;
							    }
						    }
						}
					}
				}
			}
			break;
		case 0xF20B:
			// if(MethodNo == 3)		//端口授权开启
			// {
			// 	if((pData[Offset++] == Structure_698) && (pData[Offset++] == 2))
			// 	{
			// 		if(pData[Offset++] == Unsigned_698)
			// 		{
			// 			pData[Offset++];	//端口序号

			// 			if(pData[Offset++] == Unsigned_698)
			// 			{
			// 				//pData[Offset];	//授权开始时长
			// 				DAR = DAR_Success;
			// 			}
			// 		}
			// 	}
			// }
			// else if(MethodNo == 129)	//更新配对密码
			// {
			// 	DAR = DAR_Success;
			// 	if((pData[Offset++] == Structure_698) && (pData[Offset++] == 2))
			// 	{
			// 		if(pData[Offset++] == OAD_698)
			// 		{
			// 			//pData[Offset];	//端口号OAD

			// 			Offset += 4;
			// 			if(pData[Offset++] == Visible_string_698)
			// 			{
			// 				Offset += Deal_698DataLenth( &pData[Offset], (BYTE *)&Result, ePROTOCOL_TYPE);
			// 				if ((Result!=6)&&(Result!=0))
			// 				{
			// 					DAR = DAR_HardWare;
			// 				}
			// 				else
			// 				{
			// 					if (api_VReadSafeMem(GET_SAFE_SPACE_ADDR(SafeMem_dev_work), sizeof(TSafeMem_dev_work), (BYTE *)&dev_work) == FALSE)
			// 					{
			// 						DAR = DAR_HardWare;
			// 					}
			// 					else
			// 					{
			// 						for (i = 0; i < Result; i++)
			// 						{
			// 							if (pData[Offset + i] > 9)
			// 							{
			// 								DAR = DAR_HardWare;
			// 								break;
			// 							}
			// 							pData[Offset + i] += 0x30; // 转换为ASCII存储
			// 						}
			// 						if (DAR == DAR_Success)
			// 						{
			// 							memcpy(&dev_work.Mac, &pData[Offset], Result);
			// 							dev_work.MacLen = Result;
			// 							if (api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(SafeMem_dev_work), sizeof(TSafeMem_dev_work), (BYTE *)&dev_work) == FALSE)
			// 							{
			// 								DAR = DAR_HardWare;
			// 							}
			// 							else
			// 							{
			// 								api_delayinit_ble();
			// 							}
										
			// 						}
			// 					}
			// 				}
			// 			}
			// 		}
			// 	}
			// }
			break;
		case 0xF221:
			Result=FALSE;
			if(MethodNo == 127)
			{
				if(pData[0]==Structure_698)
				{
					if(pData[1]==2)
					{
						if( pData[2] == OAD_698 )
						{
							if( pData[7] == COMDCB_698 )
							{
								if(pData[8]<17 && pData[8]>12)
								{
									Result = api_WritePara(1, (GET_STRUCT_ADDR(TFPara2, CommPara.CanBaud)), 1, &pData[8]);
									if( Result == FALSE )
									{
										DAR = DAR_HardWare;//此为固定返回，不可随意更改 与上层进行配合
									}
									else
									{
//										Serial[eCAN].OperationFlag = 1;
										DAR = DAR_Success;
									}
								}
								else
								{
									DAR = DAR_ChangeBaud;
									eResultChoice = DLT698AddOneBuf(Ch,0x00, DAR);//添加DAR
									if( eResultChoice != eADD_OK )
									{
										return eResultChoice;
									}
									eResultChoice = DLT698AddOneBuf(Ch,0x55, 00);//OPTIONAL NO CHOICE DATA 
									
									return eResultChoice;
								}
								
							}
						}
					}
				}
			}
			else if (MethodNo==128)
			{
				if (pData[0]==Structure_698)
				{
					if (pData[2]==OAD_698)
					{
						if (pData[7]==Array_698)
						{
						    if(pData[8] > MAX_CANBUS_ID_NUM)
						    {
							    Result = FALSE;
						    }
						    else
						    {
							    for(i = 0; i < pData[8]; i++)
							    {
								    // 添加节点
								    if(pData[9 + i * 2] == Unsigned_698)
								    {
									    CanNode[i] = pData[10 + 2 * i];
								    }
							    }
//							    Result = ChangeID(eADDID, CanNode, pData[8]);
							    if(Result == FALSE)
							    {
								    break;
							    }
						    }
					    }
				    }
			    }
				
			}
			else if (MethodNo==129)
			{
				if (pData[0]==Structure_698)
				{
					if (pData[2]==OAD_698)
					{
					    if(pData[8] > MAX_CANBUS_ID_NUM)
					    {
						    Result = FALSE;
					    }
					    else
					    {
						    for(i = 0; i < pData[8]; i++)
						    {
							    // 添加节点
							    if(pData[9 + i * 2] == Unsigned_698)
							    {
								    CanNode[i] = pData[10 + 2 * i];
							    }
						    }
						    Result = ChangeID(eDELID, CanNode, pData[8]);
						    if(Result == FALSE)
						    {
							    break;
						    }
					    }
				    }
			    }
				
			}
			else if (MethodNo==130)
			{
				if (pData[0]==Structure_698)
				{
					if (pData[2]==OAD_698)
					{
//						Result|=ChangeID(eCLEANID,NULL,0);
					}
					
				}
				
			}
			if (Result==TRUE)
			{
				DAR = DAR_Success;
			}
			else
			{
				DAR = DAR_RefuseOp;
			}
			
			
		default:
			break;
	}
	
	eResultChoice = DLT698AddOneBuf(Ch,0x00, DAR);//添加DAR
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	eResultChoice = DLT698AddOneBuf(Ch,0x55, 00);//OPTIONAL NO CHOICE DATA 
	
    return eResultChoice;
}
//--------------------------------------------------
//功能描述:    操作ESAM安全模式
//         
//参数:      BYTE Ch[in]          通道选择
//         
//         BYTE *pOMD[in]       OMD指针
//         
//返回值:     eAPPBufResultChoice  返回添加Buf结果       
//         
//备注内容:    无
//--------------------------------------------------
eAPPBufResultChoice ActionRequestEsamSafeMode( BYTE Ch,  BYTE *pOMD)//操作esam安全模式
{
	BYTE DAR,i;
	BYTE *pData;
	eAPPBufResultChoice eResultChoice;

	pData = &pOMD[4];//取数据指针
	DAR = DAR_WrongType;
	
	switch( pOMD[2] )
	{
	case 0x01://复位
	    if( pData[0] == Integer_698 )//数据为Integer_698
	    {
            api_ClrProSafeModePara(2);
            DAR= DAR_Success;//AR 成功
	    }
		break;
	case 127://方法127：增加显式安全模式参数（对象标识，权限）,有两参数，要用一个结构封装下传
		if( pData[0] == Structure_698 )
		{
			AlterSafeModePara(0, (void *)&(pData[2]) );//传入的缓冲从 OI_698 开始
			DAR= DAR_Success;//AR 成功	
		}
		break;
	case 128://删除显示安全属性
		if( pData[0] == OI_698)
		{
			AlterSafeModePara(1, (void *)&(pData[0]) );//传入的缓冲从 OI_698 开始
			DAR = DAR_Success;//AR 成功	
		}
		break;	
	case 129://批量增加显示安全属性
		if( pData[0] == Array_698 )
		{
			if( pData[1] <= MAX_SAFE_MODE_PARA_NUM )
			{
				for( i=0; i<pData[1]; i++ )
				{
					if( pData[4+8*i] == OI_698 )
					{
						AlterSafeModePara(0, (void *)&(pData[4+8*i]) );//传入的缓冲从 OI_698 开始
					}
					else
					{
						DAR = DAR_WrongType;
						break;
					}

				}

				if( i == pData[1] )
				{
					DAR = DAR_Success;//AR 成功	
				}
			}
		}
		break;	
	default:		
		break;
	}

	eResultChoice = DLT698AddOneBuf(Ch,0x00, DAR);//添加DAR
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	eResultChoice = DLT698AddOneBuf(Ch,0x55, 00);//OPTIONAL NO CHOICE DATA 

    return eResultChoice;
}

//--------------------------------------------------
//功能描述: 处理文件信息
//
//参数:	Ch[i]:
//		pData[in]: 698文件结构
//返回值: 报文解析长度
//
//备注:
//--------------------------------------------------
static WORD DealFileInfo(BYTE Ch, BYTE *Buf)
{
	WORD Offset = 0, Ret = 0x8000;
	WORD Len, LenOffset;
	BYTE *pDat;

	if( (Buf[Offset++] == Structure_698) && (Buf[Offset++] == 6))
	{
		if ((Buf[Offset++] == Structure_698) && (Buf[Offset++] == 6))
		{
			if (Buf[Offset] == Visible_string_698) // 源文件
			{
                Offset++;
				LenOffset = Deal_698DataLenth(&Buf[Offset], (BYTE *)&Len, ePROTOCOL_TYPE);
				pDat = &Buf[Offset + LenOffset];

				Offset = Offset + LenOffset + Len;
			}
			else if (Buf[Offset] == NULL_698)
			{
				Offset++;
			}
			else
			{
				return 0x8000;
			}

			if (Buf[Offset] == Visible_string_698) // 目标文件
			{
                Offset++;
				LenOffset = Deal_698DataLenth(&Buf[Offset], (BYTE *)&Len, ePROTOCOL_TYPE);
				pDat = &Buf[Offset + LenOffset];

				Offset = Offset + LenOffset + Len;
			}
			else if (Buf[Offset] == NULL_698)
			{
				Offset++;
			}
			else
			{
				return 0x8000;
			}
		}
		else
		{
			return 0x8000;
		}

		if(Buf[Offset++] == Double_long_unsigned_698) //文件大小
		{
			lib_ExchangeData((BYTE *)&tIap.dwFileSize, (BYTE *)&Buf[Offset], 4);

			if( tIap.dwFileSize == 0 )
			{
				return 0x8000;
			}
			Offset += 4;
		}
		else
		{
			return 0x8000;
		}

		if(Buf[Offset++] == Bit_string_698)	//文件属性
		{
			LenOffset = Deal_698DataLenth( &Buf[Offset], (BYTE *)&Len, ePROTOCOL_TYPE);
			tIap.bFileAttr = Buf[Offset + LenOffset];
			Len = (Len + 7) / 8;	//BIT String 8Bit 一个字节，

			Offset = Offset + LenOffset + Len;
		}
		else
		{
			return 0x8000;
		}

		if(Buf[Offset++] == Visible_string_698)	//文件版本
		{
			LenOffset = Deal_698DataLenth( &Buf[Offset], (BYTE *)&Len, ePROTOCOL_TYPE);
			pDat = &Buf[Offset + LenOffset];

			memcpy((BYTE *)&tIap.bFileVer[0], pDat, Len);
			Offset = Offset + LenOffset + Len;
		}
		else
		{
			return 0x8000;
		}

		if(Buf[Offset++] == Enum_698)	//文件类别
		{
			tIap.bFileType = Buf[Offset++];
		}
		else
		{
			return 0x8000;
		}
	}
	else
	{
		return 0x8000;
	}

	return Offset;
}
//--------------------------------------------------
//功能描述: 处理启动传输
//
//参数: Ch[in]: 
//		Buf[in]:
//返回值: DAR 
//
//备注:
//--------------------------------------------------
BYTE ActionStartTransfe(BYTE Ch, BYTE *Buf)
{
	DWORD dwTemp;
	WORD Offset, Len, LenOffset;
	BYTE *pDat, i, Num;

	memset( (BYTE *)&tIap, 0x00, sizeof(T_Iap));
	memset((BYTE *)&tIapInfo, 0x00, sizeof(TIapInfo));
	memset((BYTE *)&IapStatus[0], 0x00, sizeof(IapStatus));

	Offset = DealFileInfo(Ch, Buf);	//文件信息
	if(Offset == 0x8000)
	{
		return DAR_WrongType;
	}

	if((Buf[Offset] == Long_unsigned_698)||(Buf[Offset] == Double_long_unsigned_698))	//传输文件块大小
	{
		if ((Buf[Offset] == Double_long_unsigned_698))
		{
            Offset++;
			lib_ExchangeData((BYTE *)&dwTemp, (BYTE *)&Buf[Offset], sizeof(DWORD));
			if (dwTemp>0xffff)
			{
				return DAR_WrongType;
			}
			Offset += 2;
		}
        else
        {
            Offset++;
        }
		lib_ExchangeData((BYTE *)&tIap.wFrameSize, (BYTE *)&Buf[Offset], sizeof(WORD));
		Offset += 2;
		if((tIap.wFrameSize != 0) && (LEN_OF_ONE_FRAME <= tIap.wFrameSize) && (SECTOR_SIZE >= tIap.wFrameSize))
		{
			tIap.wAllFrameNo = tIap.dwFileSize / tIap.wFrameSize;
			if(tIap.dwFileSize % tIap.wFrameSize)
			{
				tIap.wAllFrameNo++;
			}

			tIap.wFrameOfOneSector = SECTOR_SIZE / tIap.wFrameSize;

			// 由文件大小和帧大小，计算是否是全部升级
			dwTemp = sizeof(IapFacCode) + FLASH_APP_END_ADDR_ROM1 - FLASH_APP_START_ADDR;
			// 防止越界
			if(ceil(1.0 * tIap.dwFileSize / tIap.wFrameSize) / (SECTOR_SIZE / tIap.wFrameSize) > SECTOR_NUM_FOR_IAP)
			{
				return DAR_WrongType;
			}

			if(dwTemp < tIap.dwFileSize)   // 超出保护区
			{
				tIap.wAllUpdate = IAP_ALL;
			}
			else
			{
				tIap.wAllUpdate = 0;
			}
		}
		else
		{
			return DAR_WrongType;
		}
	}
	else
	{
		return DAR_WrongType;
	}

	if( (Buf[Offset++] == Structure_698) && ((Buf[Offset++] == 2)))	//校验
	{
		if(Buf[Offset++] == Enum_698)	//校验类型
		{
			pDat = &Buf[Offset++];

			if(*pDat == 0 )
			{
				if (Buf[Offset++] == Octet_string_698) // 校验值
				{
					LenOffset = Deal_698DataLenth(&Buf[Offset], (BYTE *)&Len, ePROTOCOL_TYPE);
					pDat = &Buf[Offset + LenOffset];

					lib_ExchangeData((BYTE *)&tIap.wCrc16, pDat, sizeof(WORD));

					Offset = Offset + LenOffset + Len;
				}
			}
			else
			{
				return DAR_WrongType;
			}


		}
		else
		{
			return DAR_WrongType;
		}
	}
	else
	{	
		return DAR_WrongType;
	}

	if(Buf[Offset] == Array_698)	//兼容的软件版本号
	{
		Offset++;
		Num = Buf[Offset++];

		for (i = 0; i < Num; i++)
		{
			if(Buf[Offset++] == Visible_string_698)
			{
				LenOffset = Deal_698DataLenth( &Buf[Offset], (BYTE *)&Len, ePROTOCOL_TYPE);
				pDat = &Buf[Offset + LenOffset];
				if(Len > 16)	//SIZE 0 - 16
				{
					return DAR_OverRegion;
				}

				Offset = Offset + LenOffset + Len;
			}			
		}
	}
	else if (Buf[Offset] == NULL_698)
	{
		Offset++;
	}
	else
	{	
		return DAR_WrongType;
	}

	if(Buf[Offset] == Array_698)	//兼容的硬件版本号
	{
		Offset++;
		Num = Buf[Offset++];

		for (i = 0; i < Num; i++)
		{
			if(Buf[Offset++] == Visible_string_698)
			{
				LenOffset = Deal_698DataLenth( &Buf[Offset], (BYTE *)&Len, ePROTOCOL_TYPE);
				pDat = &Buf[Offset + LenOffset];
				if(Len > 16)	//SIZE 0 - 16
				{
					return DAR_OverRegion;
				}

				Offset = Offset + LenOffset + Len;
			}
		}
	}
	else if (Buf[Offset] == NULL_698)
	{
		Offset++;
	}
	else
	{
		return DAR_WrongType;
	}

	if(Buf[Offset] == Visible_string_698)		//下载方标识
	{
		Offset++;
		LenOffset = Deal_698DataLenth( &Buf[Offset], (BYTE *)&Len, ePROTOCOL_TYPE);
		pDat = &Buf[Offset + LenOffset];
		if(Len > 32)	//SIZE 2 -32
		{
			return DAR_OverRegion;
		}

		Offset = Offset + LenOffset + Len;
	}
	else if (Buf[Offset] == NULL_698)
	{
		Offset++;
	}
	tIapInfo.dwFlag = IAP_START; // 接收完启动传输命令，格式正确

	return DAR_Success;
}

//--------------------------------------------------
//功能描述: 写文件
//
//参数: Ch[in]: 
//		Buf[in]:
//返回值: DAR 
//
//备注:
//--------------------------------------------------
BYTE ActionBlockTransfe(BYTE Ch, BYTE *Buf)
{
	DWORD dwAddr, i;
	WORD Offset = 0, Len, LenOffset, wTemp, wFlag;
	BOOL boResult;
	BYTE tmp[SECTOR_SIZE];
	
	if((Buf[Offset++] == Structure_698) && (Buf[Offset++] == 2))
	{
		if(Buf[Offset++] == Long_unsigned_698)   // 块序号
		{
			lib_ExchangeData((BYTE *)&tIapInfo.wCurFrameNo, &Buf[Offset], sizeof(WORD));

			if(tIapInfo.wCurFrameNo >= (MAX_SIZE_FOR_IAP / tIap.wFrameSize))
			{
				return DAR_BlockSeq;   // 数据块序号错误
			}
			if(IapStatus[tIapInfo.wCurFrameNo / 8] & (0x01 << (tIapInfo.wCurFrameNo % 8)))   // 帧传输标志传输过
			{
				return DAR_Success;
				//return DAR_BlockSeq;   // 数据块序号错误
			}

			Offset += 2;
			if(Buf[Offset++] == Octet_string_698)   // 块数据
			{
				LenOffset = Deal_698DataLenth(&Buf[Offset], (BYTE *)&Len, ePROTOCOL_TYPE);

				boResult = FALSE;

				if(tIapInfo.wCurFrameNo == 0)   // 第一帧为确认帧，防止收到非威思顿的产品的升级包
				{
					if(memcmp(&Buf[Offset + LenOffset], &IapFacCode[0], sizeof(IapFacCode)) == 0)
					{
						tIapInfo.wFacFlag = 1;
					}
				}
				if(tIapInfo.wFacFlag == 0)
				{
					return DAR_RefuseOp;   // 如果不是威思顿的升级包，则拒收
				}
				for(i = 0; i < tIap.wFrameOfOneSector; i++)
				{
					if(IapStatus[(((tIapInfo.wCurFrameNo / tIap.wFrameOfOneSector) * tIap.wFrameOfOneSector) + i) / 8]
					   & (0x01 << (((tIapInfo.wCurFrameNo / tIap.wFrameOfOneSector) * tIap.wFrameOfOneSector) + i) % 8))
					{
						break;
					}
				}
				if(i == tIap.wFrameOfOneSector)
				{
					api_UpdateEraseFlash(IAP_CODE_ADDR + ((tIapInfo.wCurFrameNo) / tIap.wFrameOfOneSector) * SECTOR_SIZE);
				}
				// 第二帧开始存放在IAP_CODE_ADDR开始的flash区域
				dwAddr = IAP_CODE_ADDR + ((tIapInfo.wCurFrameNo) / tIap.wFrameOfOneSector) * SECTOR_SIZE + ((tIapInfo.wCurFrameNo) % tIap.wFrameOfOneSector) * tIap.wFrameSize;
				boResult = api_WriteFlashDataUpDate(dwAddr, Len, &Buf[Offset + LenOffset]);
				if(boResult == FALSE)
				{
					api_ReadMemRecordData((IAP_CODE_ADDR + ((tIapInfo.wCurFrameNo) / tIap.wFrameOfOneSector) * SECTOR_SIZE), SECTOR_SIZE, tmp);
					memcpy(tmp + (((tIapInfo.wCurFrameNo) % tIap.wFrameOfOneSector) * tIap.wFrameSize), &Buf[Offset + LenOffset], tIap.wFrameSize);
					boResult = api_WriteMemRecordData((IAP_CODE_ADDR + ((tIapInfo.wCurFrameNo) / tIap.wFrameOfOneSector) * SECTOR_SIZE), SECTOR_SIZE, tmp);
				}

				if(boResult == TRUE)
				{
					IapStatus[tIapInfo.wCurFrameNo / 8] |= 0x01 << (tIapInfo.wCurFrameNo % 8);   // 置帧传输标志
					for(wTemp = 0; wTemp < tIap.wAllFrameNo; wTemp++)
					{
						if((IapStatus[wTemp / 8] & (0x01 << (wTemp % 8))) == 0)
						{
							break;
						}
					}
					if(wTemp >= tIap.wAllFrameNo)
					{
						// 广播方式，帧已收全，则可以复位执行升级
						if(LinkData[Ch].AddressType == eBROADCAST_ADDRESS_MODE)
						{
							tIap.dwIapFlag = IAP_FLAG;
							tIap.dwCrc32 = lib_CheckCRC32((BYTE *)&tIap.bDestFile[0], sizeof(tIap) - sizeof(DWORD));   // 计算校验

							boResult = api_WriteMemRecordData(IAP_INFO_ADDR, sizeof(tIap), (BYTE *)&tIap.bDestFile[0]);
							if(boResult == TRUE)
							{
								// 延时1s后准备升级
								tIapInfo.dwFlag = IAP_READY;
								tIapInfo.dwTime = 1;
							}
						}
					}
					return DAR_Success;
				}
				else
				{
					return DAR_WrongType;
				}
			}
		}
	}

	return DAR_WrongType;
}
//--------------------------------------------------
// 功能描述:	升级倒计时
//
// 参数:	
// 返回值: 
//
// 备注:
//--------------------------------------------------
void api_IapCountDown( void )
{
	if (tIapInfo.dwTime)
	{
		tIapInfo.dwTime--;
		if (tIapInfo.dwTime == 0)
		{
			if (tIapInfo.dwFlag == IAP_READY)
			{
				tIapInfo.dwFlag = 0;
				 
				// api_PowerDownEnergy();
				Reset_CPU();
				// api_EnterLowPower(eFromOnRunEnterDownMode);
			}
		}
	}
}
//--------------------------------------------------
//功能描述:	执行升级
//
//参数:	Ch[in]: 
//		Buf[in]:
//返回值: DAR
//
//备注:
//--------------------------------------------------
BYTE ActionExeUpdate(BYTE Ch, BYTE *Buf)
{
	WORD Offset = 0, Len, LenOffset, wLen, w, wCRC16;
	BYTE *pDat, i, Num;
	DWORD dwAddr, dwTemp1, dwTemp2, dwExFlashAddr;
	TRealTimer TmpRealTimer;
	BOOL boResult;
	BYTE TmBuf[7];

	if ((Buf[Offset++] == Structure_698) && (Buf[Offset++] == 2))
	{
		if(Buf[Offset++] == DateTime_S_698)	//执行升级时间
		{
			memset(TmBuf,0xff,sizeof(TmBuf));
			if(memcmp(&Buf[Offset],TmBuf,sizeof(TmBuf)) == 0)
			{
				tIapInfo.dwTime = 2;
			}
			else
			{
				lib_InverseData(&Buf[Offset], 2);
				memcpy((BYTE *)&(TmpRealTimer.wYear), &Buf[Offset], sizeof(TRealTimer));
				dwTemp1 = api_CalcInTimeRelativeSec(&TmpRealTimer);
				dwTemp2 = api_CalcInTimeRelativeSec(&RealTimer);
				if (dwTemp1 > dwTemp2)
				{
					tIapInfo.dwTime = dwTemp1-dwTemp2;
				}
				else
				{
					return DAR_RefuseOp;
				}
			}
			
			Offset += 7;
			i = 0;
			if (Buf[Offset] == Octet_string_698 ) // 软件注册码
			{
				LenOffset = Deal_698DataLenth(&Buf[Offset], (BYTE *)&Len, ePROTOCOL_TYPE);
			}
			else if (Buf[Offset] == NULL_698)
			{
				i = 1;
			}

			if( i == 1 )
			{
				tIap.dwIapFlag = IAP_FLAG;
				tIap.dwCrc32 = lib_CheckCRC32((BYTE *)&tIap.bDestFile[0], sizeof(tIap) - sizeof(DWORD)); // 计算校验

				boResult = api_WriteMemRecordData(IAP_INFO_ADDR, sizeof(tIap), (BYTE *)&tIap.bDestFile[0]);
				if( boResult == TRUE )
				{
					tIapInfo.dwFlag = IAP_READY;

					return DAR_Success;
				}
				else 
				{
					tIapInfo.dwTime = 0;
					
					return DAR_BlockSeq;
				}
			}
		}
	}
	
	tIapInfo.dwTime = 0;
	return DAR_WrongType;
}
//--------------------------------------------------
//功能描述:    操作软件比对
//         
//参数:      BYTE Ch[in]          通道选择
//         
//         BYTE *pData[in]      pData指针
//         
//返回值:     eAPPBufResultChoice  返回添加Buf结果    
//         
//备注内容:    无
//--------------------------------------------------
eAPPBufResultChoice ActionSoftwareComparison( BYTE Ch, BYTE *pData)
{
	TTwoByteUnion Len,Leninpage;
	BYTE DAR,KeyOffest,i,factor[32],TmpBuf[32],EsamRand[100];
	BYTE *pCode_Flash,*ptr;
	WORD Result,KeyLength,Length,XorLenth,TotalLenth;
	DWORD factorAddress,DataAddress,Address;
	eAPPBufResultChoice eResultChoice;

	DAR = DAR_WrongType;
	Result = FALSE;
	KeyLength = 0;
	factorAddress = 0;
	DataAddress = 0;
	KeyOffest = 0;
	
	if( pData[0] == Structure_698 )
	{
		if( pData[1] == 5 )
		{
			if( pData[2] == Unsigned_698 )//cpu编号
			{
				if( pData[3] == 0 )//cpu编号必须为零
				{
					if( pData[4] == Unsigned_698 )//密钥索引
					{
						KeyOffest = pData[5];
						if( pData[6] == Double_long_unsigned_698 )//因子起始地址
						{
							lib_ExchangeData( (BYTE*)&factorAddress, &pData[7], 4);
							if( pData[11] == Double_long_unsigned_698 )//数据起始地址
							{
								lib_ExchangeData( (BYTE*)&DataAddress, &pData[12], 4);
								if( pData[16] == Long_unsigned_698 )//待加密数据长度
								{
									lib_ExchangeData( (BYTE*)&KeyLength, &pData[17], 2);
									DAR = DAR_Success;
								}
							}
						}
					}
				}
			}
		}
	}
    if( DAR == DAR_Success )
    {
    	XorLenth = (KeyLength/4);//获取异或的数据长度
    	
        if( XorLenth > MAX_APDU_SIZE )
        {
            DAR =  DAR_OverRegion;
        }
    	else if( (KeyLength%64) != 0 )//非64的倍数返回错误
    	{
            DAR = DAR_RefuseOp; 
    	}
    	else
    	{
            for( i=0; i < 2; i++ )
            {
                if( i == 0 )
                {
                    Address= factorAddress;
                    ptr = factor;                   //比对因子Buf指针
                    pCode_Flash = (BYTE *)factorAddress;//比对因子指针
                    Length = 16;                    //数据长度
                }
                else
                {
                    Address= DataAddress;
                    ptr = ProtocolBuf+24;                   //数据Buf指针
                    pCode_Flash = (BYTE *)DataAddress;  //数据起始指针
                    Length = KeyLength;             //数据长度
                    
                    if( Length > (sizeof(ProtocolBuf) - 24) )//防止越界
	                {
	                    break;
	                }
                }
                
                if( Address > PROGRAMEND )//大于RAM结束地址跳出
                {
                    break;
                }
            
                if( (Address+Length) > (PROGRAMEND+1) ) //结束地址补0x80
                {
                    memset( ptr, 0x00, Length);
                    memcpy( ptr, pCode_Flash, PROGRAMEND+1-Address );
                    ptr[PROGRAMEND+1-Address] = 0x80;
                }
                else if( (Address < (PROGRAMPROZ)) && ((Address+Length) > (PROGRAMPROZ)) )  //到保护区地址补0x80
                {
                    memset( ptr, 0x00, Length);
                    memcpy( ptr, pCode_Flash, PROGRAMPROZ-Address );
                    ptr[PROGRAMPROZ-Address] = 0x80;
                }
    			else if( (Address < (PROGRAMPROZ1)) && ((Address+Length) > (PROGRAMPROZ1)) )  //到保护区地址补0x80
                {
                    memset( ptr, 0x00, Length);
                    memcpy( ptr, pCode_Flash, PROGRAMPROZ1-Address );
                    ptr[PROGRAMPROZ1-Address] = 0x80;
                }
                else
                {
                    memcpy( ptr, pCode_Flash, Length );
                }

                FunctionConst(SOfTWARECOMPARISON_TASK);
            }
            
            if( i == 2)
            {
                //读ROM数据,计算密文
                lib_XorData( factor, factor+8, 8 );//得到比对因子
                
                memcpy( EsamRand, ProtocolBuf+24, 64 );
                for( i=1; i<4; i++ )
                {
                    lib_XorData( EsamRand, EsamRand+16*i, 16 );//得到data1
                }
            
                for( i=1; i<4; i++ )
                {
                    lib_XorData( ProtocolBuf+24, ProtocolBuf+24+XorLenth*i, XorLenth );//得到data1
                }


                
                TotalLenth = XorLenth+24;//数据长度+分散因子(8)+随机数(8)
                memcpy( ProtocolBuf, factor, 8 );//分散因子
                memcpy( ProtocolBuf+8, EsamRand, 16 );//随机数
                
                Result = api_SoftwareComparisonEsam( KeyOffest, TotalLenth, ProtocolBuf );
                if( Result == TRUE )
                {
                    DAR = DAR_Success;
                    Len.b[0] = ProtocolBuf[1];//获取长度
                    Len.b[1] = ProtocolBuf[0];
                    
                }

                if( Len.w < MAX_APDU_SIZE )
                {
                    memmove( ProtocolBuf, ProtocolBuf+2, Len.w );
                }
                else
                {
                    DAR = DAR_OverRegion;
                }   
                
            }
            else
            {
                DAR = DAR_WrongType;
            }
    	}
    }
	
	eResultChoice = DLT698AddOneBuf(Ch,0x00, DAR);//添加DAR
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	
	if( DAR == DAR_Success )
	{
		eResultChoice = DLT698AddOneBuf(Ch,0x00, 01);//OPTIONAL  CHOICE DATA 
		if( eResultChoice != eADD_OK )
		{
			return eResultChoice;
		}
		
		TmpBuf[0] = Structure_698;
		TmpBuf[1] = 1;
		TmpBuf[2] = Octet_string_698;
		Leninpage.w = Deal_698DataLenth( &TmpBuf[3], Len.b, eUNION_TYPE );//添加数据长度 获取数据占据的字节数
		eResultChoice = DLT698AddBuf( Ch, 0x00, TmpBuf, 3+Leninpage.w );
		if( eResultChoice != eADD_OK )
		{
			return eResultChoice;
		}
		
		eResultChoice = DLT698AddBuf( Ch, 0x55, ProtocolBuf, Len.w );
	}
	else
	{
		eResultChoice = DLT698AddOneBuf(Ch,0x00, 00);//OPTIONAL NO  CHOICE DATA 
	}
	
    return eResultChoice;

}
//--------------------------------------------------
//功能描述:    操作文件分块传输管理
//         
//参数:      BYTE Ch[in]          通道选择
//         
//         BYTE MethodNo[in]    方法号
//         
//         BYTE *pData[in]      数据指针
//         
//返回值:     eAPPBufResultChoice  返回添加Buf结果
//         
//备注内容:    无
//--------------------------------------------------
eAPPBufResultChoice ActionFileBlockTransfer( BYTE Ch,  BYTE MethodNo, BYTE *pData)
{
	eAPPBufResultChoice eResultChoice;
	BYTE DAR = 0xFF;

	eResultChoice= eAPP_ERR_RESPONSE;

    
	switch( MethodNo )
	{
		case 7://启动传输	
			DAR = DAR_NoObject;
			break;		
		case 8://写文件
			if (tIapInfo.dwFlag == IAP_START)
			{
				DAR = ActionBlockTransfe(Ch, pData);
			}
			else
			{
				DAR = DAR_RefuseOp; // 未收到正确的启动传输命令，拒绝接收升级文件
			}

			if (LinkData[Ch].AddressType == eBROADCAST_ADDRESS_MODE) // 如果是广播地址，则不应答
			{
				return eAPP_NO_RESPONSE;
			}
			break;
		case 9://读文件
			DAR = DAR_NoObject;
			break;
		case 10://软件比对
			eResultChoice = ActionSoftwareComparison( Ch, pData);
            break;
		case 13://执行升级
			DAR = ActionExeUpdate( Ch, pData);
			break;
		case 14: //启动传输
			DAR = ActionStartTransfe( Ch, pData);
			break;
		default:
			DAR = DAR_WrongType;
			break;
	}

	if(DAR != 0xFF)		//action 没有返回数据，统一添加DAR，以及选择没有数据
	{
		eResultChoice = DLT698AddOneBuf(Ch,0x00, DAR);//添加DAR
		if( eResultChoice != eADD_OK )
		{
			return eResultChoice;
		}

		eResultChoice = DLT698AddOneBuf(Ch,0x55, 0x00);//Optional 00 无数据
	}

	return eResultChoice;
}
//--------------------------------------------------
//功能描述:    操作文件传输类
//         
//参数:      BYTE Ch[in]          通道选择
//         
//         BYTE *pOMD[in]       OMD指针
//         
//返回值:     eAPPBufResultChoice  返回添加Buf结果     
//         
//备注内容:    无
//--------------------------------------------------
eAPPBufResultChoice ActionRequestFileTransfer( BYTE Ch,  BYTE *pOMD)
{
	TTwoByteUnion OI;
	BYTE MethodNo;
	BYTE *pData;
	eAPPBufResultChoice eResultChoice;
	
	lib_ExchangeData(OI.b,pOMD,2);//取oi
	MethodNo = pOMD[2];//取方法
	pData = &pOMD[4];//取data
    eResultChoice = eAPP_ERR_RESPONSE;

	switch( OI.w )
	{
		case 0xf000://文件分帧传输管理
			break;
		case 0xf001://文件分块传输管理
			eResultChoice = ActionFileBlockTransfer( Ch,  MethodNo, pData);
			break;
		case 0xf002://文件扩展传输管理
			break;
		default:
			eResultChoice = DLT698AddOneBuf(Ch,0x00, DAR_WrongType);//添加DAR
            if( eResultChoice != eADD_OK )
            {
                return eResultChoice;
            }
            
            eResultChoice = DLT698AddOneBuf(Ch,0x55, 0x00);//Optional 00 无数据
			break;
	}
	
	return eResultChoice;
}
//--------------------------------------------------
//功能描述:    操作变量类
//         
//参数:      BYTE Ch[in]          通道选择
//         
//         BYTE *pOMD[in]       OMD指针
//         
//返回值:     eAPPBufResultChoice  返回添加Buf结果
//         
//备注内容:    无
//--------------------------------------------------
eAPPBufResultChoice ActionRequestVariable( BYTE Ch, BYTE *pOMD)
{
	TTwoByteUnion OI;
	BYTE DAR,MethodNo,Type;
    BYTE i,Buf[4];
	BYTE *pData;
	eAPPBufResultChoice eResultChoice;
	
	lib_ExchangeData(OI.b,pOMD,2);//取oi
	MethodNo = pOMD[2];//取方法
	pData = &pOMD[4];//取data
	DAR = DAR_WrongType;//默认失败
	
	switch( OI.w )
	{
		case 0x2029://安时值清零
			if( (MethodNo == 1) && ( pData[0] == Integer_698) )//操作方法1且数据为Integer_698
			{
				#if( SEL_AHOUR_FUNC == YES)
				api_ClrAHour();
				DAR = DAR_Success;
				#endif
			}
			break;
		case 0x2015://跟随上报状态字的方法127（确认电能表跟随上报状态字）∷= bit-string(SIZE(32))	
			if( (MethodNo == 127) && (pData[0] == Bit_string_698) && (pData[1] == 32) ) 
			{
				lib_ExchangeBit( (BYTE*)&Buf[0], (BYTE*)&pData[2], 4);
				for( i=0; i<4; i++ )
				{
					//如果该位已上报，且命令要清这位，则清零该位
					Type = Buf[i]&ReportDataBak[Ch].Status[i];
					ReportData[Ch].Status[i] &= ~Type;
				}
				//清零备份数据
				memset(ReportDataBak[Ch].Status, 0x00, sizeof(ReportDataBak[0].Status) );
				ReportDataBak[Ch].CRC32 = lib_CheckCRC32((BYTE*)&ReportDataBak[Ch],sizeof(TReportData)-sizeof(DWORD));
				ReportData[Ch].CRC32 = lib_CheckCRC32((BYTE*)&ReportData[Ch],sizeof(TReportData)-sizeof(DWORD));

				DAR = DAR_Success;
			}
			break;
		default:
			DAR = DAR_WrongType;
			break;
	}
	
	eResultChoice = DLT698AddOneBuf(Ch,0x00, DAR);//添加DAR
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	eResultChoice = DLT698AddOneBuf(Ch,0x55, 00);//OPTIONAL NO CHOICE DATA 
    return eResultChoice;
}
//--------------------------------------------------
//功能描述:    698操作添加一个属性对象请求
//         
//参数:      BYTE *pData[in]      数据指针
//                  
//返回值:     BYTE DAR结果  
//         
//备注内容:    无
//--------------------------------------------------
BYTE ActionLCDDisplay( BYTE *pData )
{
	BYTE DAR,Buf[30],LCDSequence,Num;
	WORD Result,Time;
        
	Result = FALSE;
	DAR = DAR_WrongType;
	if( pData[0] == Structure_698 )
	{
		if( pData[1] == 3 )
		{
			if( pData[2] == CSD_698 )
			{
				if( pData[3] == 0)//选择OAD
				{
					memcpy( Buf, &pData[4],4);//取OAD
					Num = 1;
					if( pData[8] == Unsigned_698 )
					{
						LCDSequence = pData[9];
						if( pData[10] == Long_unsigned_698 )
						{
							lib_ExchangeData( (BYTE*)&Time, &pData[11], 2);
							DAR = DAR_Success;
						}
					}					
				}
				else if( pData[3] == 1)//选择ROAD
				{					
					memcpy( Buf, &pData[4], 4);

					memcpy( &Buf[4], &pData[9], 4);
					Num = 2;
					if( pData[9+4] == Unsigned_698 )
					{
						LCDSequence = pData[9+4+1];
						if( pData[9+4+2] == Long_unsigned_698 )
						{
							lib_ExchangeData( (BYTE*)&Time, &pData[9+4+3], 2);
							DAR = DAR_Success;
						}
					}
				}
			}
		}
	}

	if( DAR == DAR_Success )
	{
		Result = api_WriteLcdEchoInfo( Num, Buf, LCDSequence, Time );
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;
		}
	}
	return DAR;
}
//-----------------------------------------------
//功能描述  :    操作液晶显示
//参数:  
//           BYTE Type[in]        0 循显   1键显
//
//           BYTE Ch[in]          通道选择
//
//           BYTE *pOMD[in]       OMD指针
//
//返回值:       eAPPBufResultChoice  返回添加Buf指针
//备注内容  :    有数据返回的直接return
//-----------------------------------------------
eAPPBufResultChoice ActionRequestLCD( BYTE Type,BYTE Ch,  BYTE *pOMD)
{
	BYTE DAR;
	BYTE *pData;
	eAPPBufResultChoice eResultChoice;
	TFourByteUnion OAD;
	WORD Result,Time;
	
	pData = &pOMD[4];
	DAR = DAR_WrongType;//默认失败
	
	switch( pOMD[2] )
	{
		case 0x03://下翻
		    if( pData[0] == NULL_698 )//数据为null
		    {
                if( Type == 1 )
                {
                    api_ProcLcdCommKey( eDownKey ); 
                    DAR = DAR_Success;
                }
                else
                {
                    DAR = DAR_HardWare;
                }
		    }

			break;	
		case 0x04://上翻
            if( pData[0] == NULL_698 )//数据为null
            {
                if( Type == 1 )
                {
                    api_ProcLcdCommKey( eUpKey ); 
                    DAR = DAR_Success;
                }
                else
                {
                    DAR = DAR_HardWare;
                }
            }
		
			break;		
		case 0x05://显示查看
			DAR = ActionLCDDisplay( pData);
			break;		
		case 0x06://全显
			if( pData[0] == Long_unsigned_698 )
			{
				lib_ExchangeData( (BYTE*)&Time, &pData[1], 2 );
				OAD.d = 0XDFDFDFDF;
				Result = api_WriteLcdEchoInfo(1, OAD.b, 0, Time);
				if( Result == FALSE )
				{
					DAR = DAR_HardWare; 
				}
				else
				{
					DAR = DAR_Success; 
				}
			}
			break;
		default:
			break;
	}
	
	eResultChoice = DLT698AddOneBuf(Ch,0x00, DAR);//添加DAR
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	eResultChoice = DLT698AddOneBuf(Ch,0x55, 00);//OPTIONAL NO CHOICE DATA 
	
	return eResultChoice;
}
#if( SEL_TOPOLOGY == YES )
eAPPBufResultChoice ActionRequestTopology( BYTE Ch, BYTE *pOMD)
{
	TTwoByteUnion OI;
	BYTE DAR,MethodNo;
	BYTE *pData;
	eAPPBufResultChoice eResultChoice;
	DWORD Inpara = 0;
	
	lib_ExchangeData(OI.b,pOMD,2);//取oi
	MethodNo = pOMD[2];//取方法
	pData = &pOMD[4];//取data
	DAR = DAR_WrongType;//默认失败


	switch( MethodNo )
	{
		case 129://清空特征电流信号识别结果
			if( pData[0] == NULL_698 )
			{
				api_WriteEventAttribute(0x4E07, 0xff, (BYTE *)&Inpara, 0);
				DAR = DAR_Success;
			}
			else
			{
				DAR = DAR_OverRegion;
			}
			break;
		default:
			break;
	}

	eResultChoice = DLT698AddOneBuf(Ch,0x00, DAR);//添加DAR
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	eResultChoice = DLT698AddOneBuf(Ch,0x55, 00);//OPTIONAL NO CHOICE DATA 
	return eResultChoice;
}

eAPPBufResultChoice ActionRequestTopologySendSignal( BYTE Ch, BYTE *pOMD)
{
	TTwoByteUnion OI;
	BYTE DAR,MethodNo,Index;
	BYTE *pData;
	eAPPBufResultChoice eResultChoice;
	DWORD Inpara = 0;
	
	lib_ExchangeData(OI.b,pOMD,2);//取oi
	MethodNo = pOMD[2];//取方法
	pData = &pOMD[4];//取data
	DAR = DAR_WrongType;//默认失败


	switch( MethodNo )
	{
		case 127:	//立即启动发送
			DAR = api_SetTopoModulatSignalInfo698(0,pData,NULL);
			if( DAR == DAR_Success)
			{
				if(api_TopoSignalSendStart() == FALSE)
				{
					DAR = DAR_TempFail;
				}
				else
				{
					DAR = DAR_Success;
				}
			}
			break;
		case 128:	//定时启动发送
			if( (pData[0] != Structure_698) ||(pData[1] != 2) )
			{
				DAR = DAR_WrongType;
				break;
			}
			pData += 2;
			// 判断调制信息是否合法
			if( api_SetTopoModulatSignalInfo698(0,&pData[0],&Index) == DAR_Success)
			{
				// 判断时间是否合法
				if(pData[Index++] != DateTime_S_698)
				{
					DAR = DAR_WrongType;
					break;
				}

				lib_InverseData(&pData[Index],2);
				if( (api_CheckClock((TRealTimer*)&pData[Index]) == FALSE)
				 || (api_CalcInTimeRelativeSec((TRealTimer*)&pData[Index]) < api_CalcInTimeRelativeSec(&RealTimer))
				 || ( (api_CalcInTimeRelativeSec((TRealTimer*)&pData[Index]) - api_CalcInTimeRelativeSec(&RealTimer)) <= (TOPO_SIGNAL_SEND_INTERVAL - TopoSignalSendIntervalCnt))
				)
				{
					DAR = DAR_OverRegion;
					break;
				}
				// 设置调制信息
				DAR = api_SetTopoModulatSignalInfo698(0,&pData[0],NULL);
				if( DAR != DAR_Success)
				{
					break;
				}
				// 设置发送时间
				memcpy(&ModulatSignalInfoRAM.Time,&pData[Index],sizeof(TRealTimer));
				if(api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(ModulatSignalInfoSafeRom), sizeof(ModulatSignalInfo_t), (BYTE *)&ModulatSignalInfoRAM) == TRUE)
				{
					DAR = DAR_Success;
				}
				else
				{
					DAR = DAR_HardWare;
				}
			}
			break;
		default:
			break;
	}

	eResultChoice = DLT698AddOneBuf(Ch,0x00, DAR);//添加DAR
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	eResultChoice = DLT698AddOneBuf(Ch,0x55, 00);//OPTIONAL NO CHOICE DATA 
	return eResultChoice;
}
#endif
//-----------------------------------------------
//功能描述: 操作函数
//参数:     
//		   BYTE Ch[in]       通道选择
//
//         BYTE *pOMD[in]    输入OMD指针
//  
//返回值:     eAPPBufResultChoice 返回添加Buf结果
//
//备注内容:    带数据的直接RETRUN 不带数据的最后添加 未填下行处理数据 list不好用 操作方法时 记录数据处理长度
//-----------------------------------------------
eAPPBufResultChoice  ActionRequest( BYTE Ch,BYTE *pOMD)
{
	TTwoByteUnion OI;
	eAPPBufResultChoice eResultChoice;
	WORD DataLen;
	BYTE OMD[4],Method = 0,Mode = 0,bResult;
	TFourByteUnion OMD2;
    BYTE DTTemp[50];
	if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return eAPP_ERR_RESPONSE;
    }
    
	lib_ExchangeData(OI.b,pOMD,2);
	Method = pOMD[2];
	memcpy( OMD, pOMD, 4 );
	
	eResultChoice = DLT698AddBuf(Ch,0, pOMD, 4);//添加OAD数据 不是完整数据
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	
    DataLen = GetProtocolDataLen( pOMD+4);//获取Data长度 DataLen的错误长度在后面进行了判断
	//if(IsModuleOI(OI.w,eModuleParaOI)||IsModuleOI(OI.w,eModuleEventOI))
	if(IsModuleOI(OI.w,eModuleAllOI))
	{
		eResultChoice = (eAPPBufResultChoice)ModuleEventFunc(Ch,pOMD);
		
	}
	else if( APPData[Ch].TimeTagFlag == eTime_Invalid )//时间标签错误
    {
		eResultChoice = DLT698AddOneBuf( Ch, 0x55, DAR_TimeStamp);//DAR
		if( eResultChoice != eADD_OK )
		{
			return eResultChoice;
		}
		eResultChoice = DLT698AddOneBuf( Ch, 0x55, 0);//DATA OPTINAL 
    }
	else if(JudgeTaskAuthority( Ch, eACTION_MODE, pOMD) == FALSE)//判断安全模式参数和是否建立应用连接
	{	
		if( api_GetRunHardFlag(eRUN_HARD_MAC_ERR_FLAG) == TRUE )//挂起不允许进行操作
		{	
			if( (OI.w >= 0x8000) && (OI.w <= 0x8001) )
			{
				api_SetRelayRelayErrStatus( 0x01 );
			}
			
		    eResultChoice = DLT698AddOneBuf( Ch, 0x55, DAR_HangUp);//DAR
			if( eResultChoice != eADD_OK )
			{
				return eResultChoice;
			}
			eResultChoice = DLT698AddOneBuf( Ch, 0x55, 0);//DATA OPTINAL 
		}
		else
		{
			if( (OI.w >= 0x8000) && (OI.w <= 0x8001) )
			{
				api_SetRelayRelayErrStatus( 0x04 );
			}
			
			eResultChoice = DLT698AddOneBuf( Ch, 0x55, DAR_PassWord);//DAR
			if( eResultChoice != eADD_OK )
			{
				return eResultChoice;
			}
			eResultChoice = DLT698AddOneBuf( Ch, 0x55, 0);//DATA OPTINAL 
		}
	}
	// else if (IsModuleOI(OI.w,eModuleOrdOI ))
	// {
	// 	eResultChoice = (eAPPBufResultChoice)ModuleEventFunc( Ch, pOMD);
	// }
	
	else if( (OI.w >= 0x2000) && (OI.w <= 0x2506) )//操作变量类对象
	{
		eResultChoice = ActionRequestVariable( Ch, pOMD);
	}
	else if( (OI.w >= 0x3000) && (OI.w <= 0x330e) )//操作事件类对象
	{
		eResultChoice = ActionRequestEvent( Ch, pOMD);

		if( (Method == 4) || ( Method == 5 ) )//修改关联对象属性判断
		{
            Mode = 0x55;
		}
	}
	else if( OI.w == 0x4000 )//广播校时
	{
		eResultChoice = ActionRequestTimeBroadCast( Ch,  pOMD);
	}
	else if( OI.w == 0x4300 )//操作电气设备
	{
		eResultChoice = ActionRequestMeter( Ch,  pOMD);
	}
	#if (SEL_TOPOLOGY == YES )
	else if( OI.w == 0x4E06 )//特征电流信号发送参数
	{
		eResultChoice = ActionRequestTopologySendSignal(Ch, pOMD);
		if( (Method == 127) || (Method == 128) )
		{
			Mode = 0x55;
		}
	}
	else if( OI.w == 0x4E07 )//操作拓扑
	{
		eResultChoice = ActionRequestTopology(Ch, pOMD);
		if(Method == 129)
		{
			Mode = 0x55;
		}
	}
	#endif
	else if( (OI.w >= 0x5000) && (OI.w <= 0x5011))//操作冻结类对象 
	{
		eResultChoice = ActionRequestFreeze( Ch, pOMD);

		if( ((Method >= 4) && (Method <= 8 )) && ( Method != 6) )//修改关联对象属性判断
		{
            Mode = 0x55;
		}
	}
	else if (OI.w == 0x6000)
	{

		lib_ExchangeData(OMD2.b,&LinkData[Ch].pAPP[APPData[Ch].BufDealLen.w],4);
		
		eResultChoice=eADD_OK;
		// if (OI.w == 0x6012)
		// {
		// 	bResult = Class10_action(OMD2.d,&LinkData[Ch].pAPP[APPData[Ch].BufDealLen.w]+4,DTTemp+10,(unsigned short *)DTTemp);
		// }
		// else
		{
			bResult = Class11_action(OMD2.d,&LinkData[Ch].pAPP[APPData[Ch].BufDealLen.w]+4,DTTemp+10,(unsigned short *)DTTemp);
		}

		if(bResult == TRUE)
		{
			eResultChoice = DLT698AddOneBuf( Ch, 0x55, 00);
			eResultChoice = DLT698AddOneBuf( Ch, 0x55, 00);			
		}
		else
		{
			eResultChoice = DLT698AddOneBuf(Ch,0x00, DAR_TempFail);//添加DAR
			if( eResultChoice != eADD_OK )
			{
				return eResultChoice;
			}
			eResultChoice = DLT698AddOneBuf(Ch,0x55, 00);//OPTIONAL NO CHOICE DATA 
		}
	}
    #if (SEL_SEARCH_METER == YES )
	else if( (OI.w == 0x6002))//操作搜表
	{
		eResultChoice = ActionRequestSchelControl( Ch, pOMD);
        if( (Method == 127) || (Method == 128) )
		{
			Mode = 0x55;
		}
	}
    #endif
	else if( (OI.w >= 0x8000) && (OI.w <= 0x8001) )//继电器操作
	{
		eResultChoice = ActionRequestControl( Ch, pOMD);
	}
	else if( (OI.w >= 0xf000) && (OI.w <= 0xf002) )//操作文件传输类
	{
		eResultChoice = ActionRequestFileTransfer( Ch, pOMD);
	}
	#if(SEL_ESAM_TYPE != 0)
	else if( OI.w == 0xf100 )//操作esam 接口类
	{
		eResultChoice = ActionRequestEsam( Ch, pOMD);
		//数据更新 密钥更新 设置协商时效 开户充值 钱包初始化
		if( (Method == 7) ||(Method == 9) ||(Method == 6)||(Method == 0X0A) )
		{
            Mode = 0x55;
		}
	}
	#endif
	else if( (OI.w == 0xf300) || (OI.w == 0xf301) )//显示接口类
	{
		eResultChoice = ActionRequestLCD( (OI.w-0xf300), Ch, pOMD);
	}
	#if(SEL_ESAM_TYPE != 0)
	else if ( OI.w == 0xf101 )
	{
		eResultChoice = ActionRequestEsamSafeMode( Ch, pOMD);
		if( (Method == 127) || (Method == 129) )//增加 批量增加安全模式参数
		{
            Mode = 0x55;
		}
	}
	#endif
	else if( (OI.w >= 0xf200) && (OI.w <= 0xf221 ))//输入输出设备类
	{
		eResultChoice = ActionRequestDevice( Ch, pOMD);
		if( (OI.w == 0xf201) || (OI.w == 0xf205) )
		{
			Mode = 0x55;
		}
	}
	else//其他情况 返回DAR
	{
		eResultChoice = DLT698AddOneBuf( Ch, 0x55, DAR_NoObject);//DAR
		if( eResultChoice != eADD_OK )
		{
			return eResultChoice;
		}
		eResultChoice = DLT698AddOneBuf( Ch, 0x55, 0);//DATA OPTINAL 
	}

    if( eResultChoice == eADD_OK )
	{
	    if( Mode == 0x55 )
	    {
            if( APPData[Ch].pAPPBuf[(APPData[Ch].APPCurrentBufLen.w-2)] == DAR_Success )//将Buf回退2个字节 判断DAR 设置成功记录编程记录
            {
                api_SaveProgramRecord( EVENT_START, 0x53, OMD);
            }   
	    }
	}

	memcpy( OMD2.b, pOMD, 4 );
	if((OI.w != 0x4000)&&(OMD2.d !=0x000801F0))//为避免现场重复广播校时，广播校时不记录自由事件,升级数据包不记记录
	{
		api_WriteFreeEvent( (EVENT_METHOD_698 << 8) + pOMD[2], OI.w );
	}
    if( DataLen == 0x8000 )//如果数据解析错误，不进行后续List的解析
    {
        return eAPP_RETURN_DATA;
    }

    APPData[Ch].BufDealLen.w += (DataLen+4);
    
	return eResultChoice;	
}
//--------------------------------------------------
//功能描述:    698多个操作请求处理函数
//         
//参数:      BYTE Ch[in]       通道选择
//         
//         BYTE *pOMD[in]    OMD数据指针
//         
//返回值:     eAPPBufResultChoice 返回添加Buf结果
//         
//备注内容:    无
//--------------------------------------------------
eAPPBufResultChoice ActionRequestList( BYTE Ch,BYTE *pOMD)
{
	BYTE i,LenOffest;
	WORD NoAddress;
	eAPPBufResultChoice eResultChoice;
	TTwoByteUnion Num,No;
	
	if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return eAPP_ERR_RESPONSE;
    }
    
    LenOffest = Deal_698DataLenth( (BYTE*)&LinkData[Ch].pAPP[APPData[Ch].BufDealLen.w], Num.b, ePROTOCOL_TYPE ); 
    APPData[Ch].BufDealLen.w +=LenOffest;
	No.w = 0;
	
	eResultChoice = DLT698AddOneBuf(Ch,0, Num.b[0] );//添加个数 产生后续帧不正确
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}

	NoAddress = APPData[Ch].APPCurrentBufLen.w;
	
	for(i=0;i<Num.w;i++)
	{
		eResultChoice = ActionRequest( Ch, &LinkData[Ch].pAPP[APPData[Ch].BufDealLen.w]);
		if( eResultChoice != eADD_OK )//添加不成功就返回
		{
			return eResultChoice;
		}
		
		No.w ++;//sequence of action_result
				
		if( APPData[Ch].BufDealLen.w >= LinkData[Ch].pAPPLen.w )//已处理数据大于下发的数据进行返回，避免错误报文的干扰
		{
			break;
		}
	}
	
	JudgeAppBufLen(Ch);
	
	Deal_698SequenceOfNumData( Ch, NoAddress, No.b);
	return eResultChoice;
}
//--------------------------------------------------
//功能描述:    698操作后读取请求操作
//         
//参数:      BYTE Ch[in]          通道选择
//         
//         BYTE *pOMD[in]       OMD数据指针
//         
//返回值:     eAPPBufResultChoice  返回添加BUF结果
//         
//备注内容:  无
//--------------------------------------------------
eAPPBufResultChoice ActionThenGetRequestNormalList( BYTE Ch,BYTE *pOMD)
{
	BYTE i,LenOffest;
	WORD NoAddress;
	eAPPBufResultChoice eResultChoice;
    TTwoByteUnion Num,No;
    
    if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return eAPP_ERR_RESPONSE;
    }
    
    LenOffest = Deal_698DataLenth( (BYTE*)&LinkData[Ch].pAPP[APPData[Ch].BufDealLen.w], Num.b, ePROTOCOL_TYPE ); 
    APPData[Ch].BufDealLen.w +=LenOffest;
    No.w = 0;

	eResultChoice = DLT698AddOneBuf(Ch,0, Num.b[0] );//添加个数 产生后续帧不正确
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	
    NoAddress = APPData[Ch].APPCurrentBufLen.w;

	for(i=0;i<Num.w;i++)
	{
		//设置数据
		eResultChoice = ActionRequest( Ch,&LinkData[Ch].pAPP[APPData[Ch].BufDealLen.w] );//
		if( eResultChoice != eADD_OK )//添加不成功就返回
		{
			return eResultChoice;
		}
		if( APPData[Ch].BufDealLen.w >= LinkData[Ch].pAPPLen.w )//已处理数据大于下发的数据进行返回，避免错误报文的干扰
		{
			break;
		}
		//读取数据
		eResultChoice= GetRequestNormal( Ch, &LinkData[Ch].pAPP[APPData[Ch].BufDealLen.w]);
		if( eResultChoice != eADD_OK )//添加不成功就返回
		{
			return eResultChoice;
		}
		
        APPData[Ch].BufDealLen.w += 1;//读取延时
        
		No.w++;// sequence of action_then_get_result
		
		if( APPData[Ch].BufDealLen.w >= LinkData[Ch].pAPPLen.w )//已处理数据大于下发的数据进行返回，避免错误报文的干扰
		{
			break;
		}
	}

    JudgeAppBufLen(Ch);
    
	Deal_698SequenceOfNumData( Ch, NoAddress, No.b);
	
    return eResultChoice;
}
//--------------------------------------------------
//功能描述:    698设置请求
//         
//参数:      BYTE Ch[in]      通道选择
//         
//         BYTE *pAPDU[in]  APDU指针
//         
//返回值:     无  
//         
//备注内容:    无
//--------------------------------------------------
void DealSet_Request(BYTE Ch,BYTE *pAPDU )
{
	eAPPBufResultChoice eResultChoice;
	
	if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return;
    }
    
	DLT698AddBuf(Ch, 0, LinkData[Ch].pAPP,3);//添加命令头
	APPData[Ch].BufDealLen.w = 3;
    eResultChoice = eAPP_ERR_RESPONSE;

	// 低功耗运行模式
	if(api_GetSysStatus(eSYS_STATUS_RUN_LOW_POWER) == TRUE)
	{
		APPData[Ch].APPFlag = APP_ERR_DATA;
		return;
	}
	switch(pAPDU[1])
	{		
	case SET_REQUEST_NORMAL:
		eResultChoice = SetRequestNormal( Ch, (BYTE  *)&(pAPDU[3]));
		break;
	case SET_REQUEST_NORMALLIST:	
		eResultChoice = SetRequestNormalList( Ch );
		break;
	case SET_THEN_GET_REQUEST:
		eResultChoice = SetThenGetRequestNormalList(Ch);
		break;
	default:
		break;		
	}
	
    if( eResultChoice == eAPP_NO_RESPONSE )
	{
		APPData[Ch].APPFlag = APP_NO_DATA;
	}
	else if( eResultChoice == eAPP_ERR_RESPONSE )
	{
	    APPData[Ch].APPFlag = APP_ERR_DATA;
	}
	else
	{
		APPData[Ch].APPFlag = APP_HAVE_DATA;
	}
}
//--------------------------------------------------
//功能描述:  
//         
//参数:      
//
//返回值:    
//         
//备注:  仅第一次分帧调用
//--------------------------------------------------
BOOL  SetNextPara(  BYTE Ch,void* pgetcfun,DWORD oadd,WORD offset,BYTE *pPara,BYTE GetrequsetNextType)
{
    memset(&pGetRequestNext[Ch],0x00,sizeof(TGetRequestNext));
	if (pPara!=NULL)
	{
		memcpy(pGetRequestNext[Ch].pPara,pPara,sizeof(pGetRequestNext[Ch].pPara));
	}
	pGetRequestNext[Ch].pgetcfun=pgetcfun;
	pGetRequestNext[Ch].offset=offset;
	pGetRequestNext[Ch].oadd=oadd;
	pGetRequestNext[Ch].GetrequsetNextType=GetrequsetNextType;
	pGetRequestNext[Ch].FZtype=TRUE;
	return TRUE;
}
//--------------------------------------------------
//功能描述:  GetRequestNext
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
eAPPBufResultChoice  GetRequestNext( BYTE Ch)
{
	WORD Len;
	eAPPBufResultChoice eResultChoice;
	BOOL bNext=FALSE;
	WORD PwRNum;
	BYTE Tempoad[4];

	if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return eAPP_ERR_RESPONSE;
    }
    
	pGetRequestNext[Ch].FZtype=TRUE;

	lib_ExchangeData(Tempoad,(BYTE*)&pGetRequestNext[Ch].oadd,4);
	eResultChoice = DLT698AddBuf(Ch,0,Tempoad, 4);//添加OAD数据 不是完整数据
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	
    APPData[Ch].BufDealLen.w +=4;//已处理4字节oad

	if( (pGetRequestNext[Ch].pgetcfun==(void*)Class11_Get_rec)
//	   ||(pGetRequestNext[Ch].pgetcfun==(void*)Class10_Get_rec)
	   )
	{
		Len = ((FW_BDHWHWHH)pGetRequestNext[Ch].pgetcfun)(Ch, pGetRequestNext[Ch].oadd,pGetRequestNext[Ch].pPara+4,
														 pGetRequestNext[Ch].offset,&APPData[Ch].pAPPBuf[APPData[Ch].APPCurrentBufLen.w],
														 (APPData[Ch].APPMaxBufLen.w- APPData[Ch].APPCurrentBufLen.w),&bNext,&PwRNum );
		pGetRequestNext[Ch].GetrequsetNextType = 2;
	}
	else if( (pGetRequestNext[Ch].pgetcfun==(void*)Class11_Get)
//			||(pGetRequestNext[Ch].pgetcfun==(void*)Class10_Get)
			)
	{
		Len = ((FW_DWHWH)pGetRequestNext[Ch].pgetcfun)( pGetRequestNext[Ch].oadd,pGetRequestNext[Ch].offset,
													   &APPData[Ch].pAPPBuf[APPData[Ch].APPCurrentBufLen.w+1],
													   (APPData[Ch].APPMaxBufLen.w- APPData[Ch].APPCurrentBufLen.w-1),&bNext );
		PwRNum=APPData[Ch].pAPPBuf[APPData[Ch].APPCurrentBufLen.w+2] + pGetRequestNext[Ch].offset;
		pGetRequestNext[Ch].GetrequsetNextType = 1;
		if (Len)
		{
			eResultChoice = DLT698AddOneBuf(Ch,0, 1);
		}
		
	}
	else
	{
		return eAPP_ERR_RESPONSE;
	}
	if(Len)
	{
		pGetRequestNext[Ch].SlicingIndex++;
		pGetRequestNext[Ch].offset = PwRNum;
		pGetRequestNext[Ch].IslastFrame = (!bNext);
		APPData[Ch].BufDealLen.w += Len;
		APPData[Ch].APPCurrentBufLen.w+=Len;
		return eADD_OK;
	}
	else
	{
		return eAPP_ERR_RESPONSE;
	}
	
	
	
}
//--------------------------------------------------
//功能描述:    698读取请求
//         
//参数:      BYTE Ch[in]       通道选择
//         
//         BYTE *pAPDU[in]   APDU指针
//         
//返回值:     无 
//         
//备注内容:    无
//--------------------------------------------------
static void DealGet_Request(BYTE Ch,BYTE *pAPDU)
{
	eAPPBufResultChoice eResultChoice;

	if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return;
    }

	DLT698AddBuf(Ch, 0, LinkData[Ch].pAPP,3);//添加命令头
	APPData[Ch].BufDealLen.w = 3;
	APPData[Ch].APPCurrentBufLen.w+=5;
	eResultChoice = eAPP_ERR_RESPONSE;
	
	switch(pAPDU[1])
	{		
	case GET_REQUEST_NORMAL:
		eResultChoice = GetRequestNormal(Ch,&pAPDU[3]);
		break;
	case GET_REQUEST_NORMALLIST:	
		eResultChoice = GetRequestNormalList(Ch);
		break;
	case GET_REQUEST_RECORD:
		eResultChoice = GetRequestRecord(Ch,&pAPDU[3]);
		break;
	case GET_REQUEST_RECORDLIST:	 
		eResultChoice = GetRequestRecordList(Ch);
		break;
	case GET_REQUEST_NEXT:
		eResultChoice = GetRequestNext(Ch);
		break;
	default:
		break;
	}
	if (pGetRequestNext[Ch].FZtype==TRUE)
	{
		pGetRequestNext[Ch].FZtype=FALSE;
		APPData[Ch].pAPPBuf[1]=0x05;//分帧响应一个数据块
		APPData[Ch].pAPPBuf[3]=pGetRequestNext[Ch].IslastFrame;
		APPData[Ch].pAPPBuf[4]=pGetRequestNext[Ch].SlicingIndex/0x100;
		APPData[Ch].pAPPBuf[5]=pGetRequestNext[Ch].SlicingIndex%0x100;
		APPData[Ch].pAPPBuf[6]=pGetRequestNext[Ch].GetrequsetNextType;
		APPData[Ch].pAPPBuf[7]=1;//暂固定为1  不支持list的应用层分帧
		if (pGetRequestNext[Ch].IslastFrame)
		{
			memset(&pGetRequestNext[Ch],0x00,sizeof(TGetRequestNext));
		}
	}
	else
	{
		APPData[Ch].APPCurrentBufLen.w -=5;
		memcpy( &APPData[Ch].pAPPBuf[3],&APPData[Ch].pAPPBuf[3+5],APPData[Ch].APPCurrentBufLen.w-3);
		memset(&pGetRequestNext[Ch],0x00,sizeof(TGetRequestNext));
	}
	
	
    if( eResultChoice == eAPP_NO_RESPONSE )
	{
		APPData[Ch].APPFlag = APP_NO_DATA;
	}
	else if( eResultChoice == eAPP_ERR_RESPONSE )
	{
	    APPData[Ch].APPFlag = APP_ERR_DATA;
	}
	else
	{
		APPData[Ch].APPFlag = APP_HAVE_DATA;
	}
}
//--------------------------------------------------
//功能描述:    698操作请求函数
//         
//参数:      BYTE Ch[in]    通道选择
//         
//         BYTE *pAPDU[in] apdu指针
//         
//返回值:     无 
//         
//备注内容:    无
//--------------------------------------------------
void DealAction_Request(BYTE Ch,BYTE *pAPDU)
{
	eAPPBufResultChoice 	eResultChoice;
	
	if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
	{
        return;
    }

	DLT698AddBuf(Ch, 0, LinkData[Ch].pAPP,3);//添加命令头
	APPData[Ch].BufDealLen.w = 3;
	eResultChoice = eAPP_ERR_RESPONSE;
	
	// 低功耗运行模式
	if(api_GetSysStatus(eSYS_STATUS_RUN_LOW_POWER) == TRUE)
	{
		APPData[Ch].APPFlag = APP_ERR_DATA;
		return;
	}

	switch(pAPDU[1])
	{		
	case ACTION_REQUEST_NORMAL:
		eResultChoice = ActionRequest( Ch,(BYTE  *)&(pAPDU[3]) );
		break;
	case ACTION_REQUEST_NORMALLIST:	
		eResultChoice = ActionRequestList( Ch,(BYTE  *)&(pAPDU[3]) );
		break;
	case ACTION_THEN_GET_REQUEST:
		eResultChoice = ActionThenGetRequestNormalList( Ch,(BYTE  *)&(pAPDU[3]));
		break;	
	default:
		break;		
	}
	
    if( eResultChoice == eAPP_NO_RESPONSE )
	{
		APPData[Ch].APPFlag = APP_NO_DATA;
	}
	else if( eResultChoice == eAPP_ERR_RESPONSE )
	{
	    APPData[Ch].APPFlag = APP_ERR_DATA;
	}
	else
	{
		APPData[Ch].APPFlag = APP_HAVE_DATA;
	}
}

//--------------------------------------------------
//功能描述:    698上报确认函数
//         
//参数:      BYTE Ch[in]    通道选择
//         
//         BYTE *pAPDU[in] apdu指针
//         
//返回值:     无 
//         
//备注内容:    无
//--------------------------------------------------
void DealReportResponse_Request(BYTE Ch,BYTE *pAPDU)
{
	BYTE i, Num;
	TFourByteUnion OAD;
	
	if( Ch != eCR )//非载波通道不进行处理
	{
		return ;
	}

	switch(pAPDU[1])
	{		
		case REPORT_RESPONSE_LIST:
		case REPORT_RESPONSE_RECORD_LIST:
		Num = pAPDU[3];//取个数
		if( Num < 10 )//最多支持确认10个OAD
		{
			for( i=0; i < Num; i++ )
			{
				lib_ExchangeData( OAD.b, &pAPDU[4+4*i], 4 );
				if( OAD.d == 0x33200200 )//确认事件新增列表
				{
					api_ResponseActiveReport( 0x00 );
				}
				else if( OAD.d == 0x20150200 )//确认跟随上报状态字
				{
					api_ResponseActiveReport( 0x55 );
				}
				else if( OAD.d == 0x30110201 )//确认掉电事件
				{
					api_ResponseActiveReport( 0xAA );
				}
				else
				{}
			}
		}
		
		break;
		
		default:
		break;
	}

	APPData[Ch].APPFlag = APP_NO_DATA;
	
}

//--------------------------------------------------
//功能描述:  698代理请求函数
//         
//参数:      BYTE Ch[in]    通道选择
//         
//           BYTE *pAPDU[in] apdu指针
//         
//返回值:     无 
//         
//备注内容:    无
//--------------------------------------------------
// void DealProxy_Request( BYTE Ch,BYTE *pAPDU )
// {
// 	BOOL result = FALSE;

// 	if((Ch >= MAX_COM_CHANNEL_NUM)
// 	||(pAPDU == NULL)
// 	||(JLTxBuf.byValid != JLS_IDLE))
// 	{
// 		APPData[Ch].APPFlag  = APP_NO_DATA;
//         return;
//     }

// 	JLTxBufInit(&JLTxBuf);

// 	switch(pAPDU[1])
// 	{		
//         case PROXY_GET_REQUEST_LIST:
//             result = ProxyGetRequestList( Ch,&JLTxBuf );
//             break;
//         case PROXY_GET_REQUEST_RECORD:	
//             result = ProxyGetRequestRecord( Ch,&JLTxBuf );
//             break;
//         case PROXY_SET_REQUEST_LIST:
//             result = FALSE;
//             break;	
//         case PROXY_SET_THEN_GET_REQUEST_LIST:
//             result = FALSE;
//             break;
//         case PROXY_ACTION_REQUEST_LIST:
//             result = FALSE;
//             break;
//         case PROXY_ACTION_THEN_GET_REQUEST_LIST:
//             result = FALSE;
//             break;
//         case PROXY_TRANS_COMMAND_REQUEST:
//             result = ProxyTransCommandRequest( Ch,&JLTxBuf );
//             break;
//         case PROXY_INNER_TRANS_COMMAND_REQUEST:
//             result = FALSE;
//             break;
//         default:
//             result = FALSE;
//             break;		
// 	}
	
//     if(result == TRUE)
//     {
//         JLTxBuf.bySourceNo   = Ch;
//         JLTxBuf.wRxWptr	     = 0;
//         JLTxBuf.SafeMode     = APPData[Ch].eSafeMode;
//         JLTxBuf.byValid      = JLS_TX;
//         APPData[Ch].APPFlag  = APP_NO_DATA;
//         //超时时间防护
//         if(JLTxBuf.wTimeOut == 0)
//         {
//             JLTxBuf.wTimeOut = 5;
//         }
//         else if( (JLTxBuf.byDestNo == eCR) && (JLTxBuf.wTimeOut > 30) )
//         {
//             JLTxBuf.wTimeOut = 30;
//         }
//         else if((JLTxBuf.byDestNo != eCR) &&(JLTxBuf.wTimeOut > 10) )
//         {
//             JLTxBuf.wTimeOut = 10;
//         }
//     }
//     else
//     {
//         JLTxBufInit(&JLTxBuf);
//         APPData[Ch].APPFlag  = APP_ERR_DATA;
//     }
// }
static WORD AnalyseGetRequestRecordLen( BYTE *pOAD )
{
    WORD Len,Len1;
    BYTE i;
    Len = 4;
    
    if( pOAD[Len] == 1 )
    {
        Len += 5;
        Len1 = GetProtocolDataLen( (BYTE*)&pOAD[Len] );//获取DATA的数据长度
        if( Len1 == 0x8000 )//如果获取长度失败，暂时定为程序无应答
        {
            return 0X8000;
        }
        Len += Len1;
        
        if( pOAD[Len] == 0 )//RCSD
        {
            Len +=1;
        }
        else
        {
            if( pOAD[Len+1] == 0 )
            {
                Len += (5*pOAD[Len]+1);
            }
            else if( pOAD[Len+1] == 1 )
            {
                Len += ((1+4+1+pOAD[Len+6]*4)*pOAD[Len]+1);
            }
            else
            {
                return 0X8000;
            }
        }
    }
    else if( pOAD[Len] == 2 )
    {
        Len += 5;

        Len1 = GetProtocolDataLen( (BYTE*)&pOAD[Len] );//获取起始值长度
        if( Len1 == 0x8000 )//如果获取长度失败，暂时定为程序无应答
        {
            return 0x8000;
        }
        
        Len += Len1; 
        
        Len1 = GetProtocolDataLen( (BYTE*)&pOAD[Len] );//获取结束值长度
        if( Len1 == 0x8000 )//如果获取长度失败，暂时定为程序无应答
        {
            return 0x8000;
        }
        
        Len += Len1; 

        Len1 = GetProtocolDataLen( (BYTE*)&pOAD[Len] );//获取数据间隔
        if( Len1 == 0x8000 )//如果获取长度失败，暂时定为程序无应答
        {
            return 0x8000;
        }
        
        Len += Len1; 
        
        if( pOAD[Len] == 0 )//RCSD
        {
            Len +=1;
        }
        else
        {
            if( pOAD[Len+1] == 0 )
            {
                Len += (5*pOAD[Len]+1);
            }
            else if( pOAD[Len+1] == 1 )
            {
                Len += ((1+4+1+pOAD[Len+6]*4)*pOAD[Len]+1);
            }
            else
            {
                return 0X8000;
            }
        }

    }
	else if(pOAD[Len] == 7)
	{
		Len += 1;
		Len += 18;
		if(pOAD[Len] == 0)   // RCSD
		{
			Len += 1;
		}
		else
		{
			Len1 = Len + 1;
			for ( i = 0; i < pOAD[Len]; i++)
			{
				if(pOAD[Len1] == 0)
				{
					Len1 += 5;
				}
				else if(pOAD[Len1] == 1)
				{
					Len1 += (1 + 4 + 1 + pOAD[Len1 + 5] * 4);
				}
				else
				{
					return 0X8000;
				}
			}
			Len = Len1;
		}
	}
	else if( pOAD[Len] == 9 )
    {
        Len += 2;

        if( pOAD[Len] == 0 )//RCSD
        {
            Len +=1;
        }
        else
        {
            if( pOAD[Len+1] == 0 )
            {
                Len += (5*pOAD[Len]+1);
            }
            else if( pOAD[Len+1] == 1 )
            {
                Len += ((1+4+1+pOAD[Len+6]*4)*pOAD[Len]+1);
            }
            else
            {
                return 0X8000;
            }
        }

    }
    
    return Len;
}

//--------------------------------------------------
//功能描述:    698读取请求
//         
//参数:      BYTE Ch[in]       通道选择
//         
//         BYTE *pAPDU[in]   APDU指针
//         
//返回值:     无 
//         
//备注内容:    无
//--------------------------------------------------
static WORD AnalyseGetRequestLen( BYTE *pAPDU )
{
    WORD Len,Len1;
    BYTE i;
    RSD rsd;

	Len = 3;

	switch(pAPDU[1])
	{		
    case GET_REQUEST_NEXT:
		Len += 2;
		break;
	case GET_REQUEST_NORMAL:
		Len += 4;
		break;
	case GET_REQUEST_NORMALLIST:	
		Len += pAPDU[3]*4+1;
		break;
	case GET_REQUEST_RECORD:
		Len += 4;
		// 1、选择方法
		Len1 = gdw698buf_RSD(&pAPDU[Len], &rsd, TD12);
		// 2、选择内容
		Len1 += objGetDataLen(dt_RCSD, &pAPDU[Len + Len1], MAX_NUM_NEST);
//		 Len1 = AnalyseGetRequestRecordLen( &pAPDU[Len] );
		if((Len1 == 0) || (Len1 == 0x8000))
		{
			return 0x8000;
		}
		Len += Len1;
		break;
	case GET_REQUEST_RECORDLIST:	 
	    Len += 1;
	    for( i = 0; i < pAPDU[3]; i++ )
	    {
            Len1 = AnalyseGetRequestRecordLen( &pAPDU[Len] );
            if( Len1 == 0x8000 )
            {
                return 0x8000;
            }
            Len += Len1;
	    }
		break;
	default:
		return 0x8000;
	}

	return Len;
}
//--------------------------------------------------
//功能描述:    698读取请求
//         
//参数:      BYTE Ch[in]       通道选择
//         
//         BYTE *pAPDU[in]   APDU指针
//         
//返回值:     无 
//         
//备注内容:    无
//--------------------------------------------------
static WORD AnalyseSetRequestNormalLen( BYTE *pOAD )
{ 
    WORD Len,Len1;

    Len = 4;
    Len1 = GetProtocolDataLen( pOAD+Len);//获取Data长度 DataLen的错误长度在后面进行了判断
    if( Len1 == 0x8000 )
    {
        return 0x8000;
    }

    Len += Len1;

    return Len;
}

//--------------------------------------------------
//功能描述:    698读取请求
//         
//参数:      BYTE Ch[in]       通道选择
//         
//         BYTE *pAPDU[in]   APDU指针
//         
//返回值:     无 
//         
//备注内容:    无
//--------------------------------------------------
static WORD AnalyseSetRequestLen( BYTE *pAPDU )
{
    WORD Len,Len1;
    BYTE i;
    
	Len = 3;

	switch(pAPDU[1])
	{		
	case SET_REQUEST_NORMAL:
		Len += AnalyseSetRequestNormalLen( (BYTE  *)&(pAPDU[3]));
		break;
	case SET_REQUEST_NORMALLIST:
	    Len += 1;
	    for( i = 0; i< pAPDU[3]; i++ )
	    {
            Len1 = AnalyseSetRequestNormalLen( (BYTE  *)&(pAPDU[Len]));
            if( Len1 == 0x8000 )
            {
                return 0x8000;
            }

            Len += Len1;
	    }
		break;
	case SET_THEN_GET_REQUEST:
	    Len += 1;
	    for( i = 0; i< pAPDU[3]; i++ )
	    {
            Len1 = AnalyseSetRequestNormalLen( (BYTE  *)&(pAPDU[Len]));
            if( Len1 == 0x8000 )
            {
                return 0x8000;
            }

            Len += Len1+5;
	    }
		break;
	default:
		break;		
	}

    return Len;
}

//--------------------------------------------------
//功能描述:    698上报响应
//         
//参数:      BYTE Ch[in]       通道选择
//         
//         BYTE *pAPDU[in]   APDU指针
//         
//返回值:     无 
//         
//备注内容:    无
//--------------------------------------------------
static WORD AnalyseResponseRequestLen( BYTE *pAPDU )
{
	WORD Len;

	Len = 1;
	
	switch(pAPDU[1])
	{		
	case REPORT_RESPONSE_LIST:
	case REPORT_RESPONSE_RECORD_LIST:
	    Len += 4;
	    Len += (pAPDU[3]*4);
		break;
	default:
		break;		
	}

    return Len;
}

//--------------------------------------------------
//功能描述:  获取698代理请求时间戳的位置
//         
//参数:      BYTE *pAPDU[in]   APDU指针
//         
//返回值:     无 
//         
//备注内容:    无
//--------------------------------------------------
static WORD AnalyseProxyRequestLen( BYTE *pAPDU )
{
	BYTE requestItemNum  = 0;
	WORD Len = 0x8000;
	
	if(pAPDU == NULL)
	{
		Len = 0x8000;
		return Len;
	}

	switch(pAPDU[1])
	{		
		case PROXY_GET_REQUEST_LIST:
			Len = 5;
			requestItemNum = pAPDU[Len++];
			for(BYTE i = 0;i<requestItemNum;i++)
			{
				Len += (1+pAPDU[Len]);		//TSA
				Len += 2;					//Server TimeOut
				Len += (1+pAPDU[Len]*4);	//OAD
			}
			break;
		case PROXY_GET_REQUEST_RECORD:	
			Len = 5;
			Len += (1+pAPDU[Len]);			//TSA
            Len += 4;                       //OAD
			Len += objGetDataLen(dt_RSD,&pAPDU[Len],MAX_NUM_NEST);
			Len += objGetDataLen(dt_RCSD,&pAPDU[Len],MAX_NUM_NEST);
			break;
		case PROXY_SET_REQUEST_LIST:
			Len = 0x8000;
			break;	
		case PROXY_SET_THEN_GET_REQUEST_LIST:
			Len = 0x8000;
			break;
		case PROXY_ACTION_REQUEST_LIST:
			Len = 0x8000;
			break;
		case PROXY_ACTION_THEN_GET_REQUEST_LIST:
			Len = 0x8000;
			break;
		case PROXY_TRANS_COMMAND_REQUEST:
			Len = 3;
			Len += 4;						//OAD
			Len += 5;						//COMDCB
			Len += 2;						//等待接受报文超时时间（s）
			Len += 2;						//等待接收字节超时时间（ms）
			if(pAPDU[Len] < 0x7F)			//数据长度小于127
            {
                Len += (pAPDU[Len]+1);
            }
            else if(pAPDU[Len] == 0x81)		//长度大于128小于255
            {   
				Len += (pAPDU[Len+1]+2);
            }
            else if(pAPDU[Len] == 0x82)		//长度大于255
            {
                Len += (pAPDU[Len+1]<<8 + pAPDU[Len+2] + 3);
            }
			else
			{
				Len = 0x8000;
			}
			break;
		case PROXY_INNER_TRANS_COMMAND_REQUEST:
			Len = 0x8000;
			break;
		default:
			Len = 0x8000;
			break;		
	}

    return Len;
}
//--------------------------------------------------
//功能描述: 递归APDU层数据长度
//         
//参数:      BYTE Ch[in]       通道选择
//         
//返回值:     无 
//         
//备注内容:    无
//--------------------------------------------------
static WORD AnalyseDlt698ApduLen( BYTE Ch)
{
    WORD Len = 0,Len1 = 0;
    TTwoByteUnion TmpLen,TmpLen1;
    
	if( APPData[Ch].eSafeMode != eSECURITY_FAIL )//解密失败不进行处理
	{
		switch( LinkData[Ch].pAPP[0] ) //cmd
		{
		case CONNECT_REQUEST:
		    if( LinkData[Ch].pAPP[39] == 0 )//公共连接
		    {
                Len = 39+1;
		    }
		    else if( LinkData[Ch].pAPP[39] == 1 )//一般密码
		    {
		        /*Len1 = GetProtocolDataLen((BYTE*)&LinkData[Ch].pAPP[40]);//visible-string
		        if( Len1 == 0x8000 )
		        {
                    return 0x8000;
		        }*/
		        Len1 = Deal_698DataLenth( (BYTE*)&LinkData[Ch].pAPP[40], TmpLen.b, ePROTOCOL_TYPE ); 
                Len = 39+1+Len1+TmpLen.w;
		    }
		    else if( LinkData[Ch].pAPP[39] == 2 )//对称加密
		    {
		        Len = Deal_698DataLenth( (BYTE*)&LinkData[Ch].pAPP[40], TmpLen.b, ePROTOCOL_TYPE );
		        Len1 = Deal_698DataLenth( (BYTE*)&LinkData[Ch].pAPP[40+Len+TmpLen.w], TmpLen1.b, ePROTOCOL_TYPE );
                Len = 39+1+Len+TmpLen.w+Len1+TmpLen1.w;//对称加密
		    }
		    else
		    {
               return 0x8000;
		    }
			break;
		case RELEASE_REQUEST:
			Len = 2;
			break;
		case GET_REQUEST:
			Len1 = AnalyseGetRequestLen( (BYTE*)&LinkData[Ch].pAPP[0] );
            if( Len1 == 0x8000 )
            {
                return 0x8000;
            }
            Len = Len1;
			break;	
		case SET_REQUEST:
		case ACTION_REQUEST:
			Len1 = AnalyseSetRequestLen( (BYTE*)&LinkData[Ch].pAPP[0] );
			if( Len1 == 0x8000 )
            {
                return 0x8000;
            }
            Len = Len1;
			break;		
		case REPORT_RESPONSE:
			Len1 = AnalyseResponseRequestLen( (BYTE*)&LinkData[Ch].pAPP[0] );
			if( Len1 == 0x8000 )
            {
                return 0x8000;
            }
            Len = Len1;
			
            break;
        // case PROXY_REQUEST:
		// 	Len1 = AnalyseProxyRequestLen( (BYTE*)&LinkData[Ch].pAPP[0] );
		// 	if( Len1 == 0x8000 )
        //     {
        //         return 0x8000;
        //     }
        //     Len = Len1;
			
        //     break;
		default:
		    return 0x8000;
		}
	}

	return Len;
}
//--------------------------------------------------
//功能描述:    处理跟随上报状态字
//         
//参数:      BYTE Ch[in]      通道选择
//         	
//		   BYTE *pBuf[out]	保存数据的缓冲
//
//返回值:     无 
//         
//备注内容:    无论数据是否填充成功都需进行返回
//--------------------------------------------------
WORD DealFollowReportStatus( BYTE Ch, BYTE *pBuf)
{
	BYTE *pByte;
    WORD Result;
    
    if( ReadReportFlag( eFollowStatusReportMethod ) == 0 )//上报状态字上报方式为主动上报
    {
		return FALSE;
    }
    
	pByte = (BYTE *)&(ReportData[Ch].Status[0]);
	if( (pByte[0] ==0x00) && (pByte[1] ==0x00) && (pByte[2] ==0x00) && (pByte[3] ==0x00) )
	{
		return FALSE;
	}

	pBuf[0] = 0x20;//跟随上报状态字OAD
	pBuf[1] = 0x15;
	pBuf[2] = 0x02;
	pBuf[3] = 0x00;
	pBuf[4] = 0x01;//data
	pBuf[5] = Bit_string_698;
	pBuf[6] = 32;
	Result = api_ReadFollowReportStatus_Mode(Ch, 0, (BYTE*)&pBuf[7]);
	if( Result == 0x8000 )
	{
        return FALSE;
	}
	lib_ExchangeBit( (BYTE*)&pBuf[7], (BYTE*)&pBuf[7], 4 );
	
	return TRUE;
}
//--------------------------------------------------
//功能描述:    处理跟随上报
//         
//参数:      BYTE Ch[in]  通道选择
//         
//返回值:     无 
//         
//备注内容:    无论数据是否填充成功都需进行返回
//--------------------------------------------------
void DealFollowReport( BYTE Ch)
{
	BYTE Buf[30];
    WORD Len,Result;
    
    if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return;
    }
    
    //不允许跟随上报、红外通道、不支持上报通道 不添加跟随上报
	if( (ReadReportFlag(eFollowReport) == FALSE)
	  // ||( Ch == eIR) 
	   ||(api_GetReportChannelStatus(Ch) == FALSE) )
	{
		 DLT698AddOneBuf(Ch,0x77, 00);//添加00 无主动上报
		 return;
	}

	Len = ( MAX_APDU_SIZE -APPData[Ch].APPCurrentBufLen.w);//判断剩余buf长度 使用最大长度进行判断 不低于13个字节代表可以填跟随上报

	if( Len >= (13+11))//剩余长度可进行第一次的组帧
	{
		Buf[0] = 1;//optional
		Buf[1] = 1;//choice of sequence of  A-ResultNormal
		Buf[2] = 1;//sequence of  A-ResultNormal
		Buf[3] = 0x33;//跟随上报OAD
		Buf[4] = 0x20;
		Buf[5] = 0x02;
		Buf[6] = 0x01;
		Buf[7] = 0x01;//data
		Buf[8] = OAD_698;//OAD数据类型
		Result = GetFirstReportOad( 0, Ch, Buf+9 );//OAD
		if( Result == FALSE )
		{			
			if( DealFollowReportStatus(Ch, Buf+3) == FALSE )
			{
				DLT698AddOneBuf(Ch,0x77, 00);//添加00 无主动上报
			}
			else
			{
				DLT698AddBuf( Ch, 0x77, Buf, (3+11) );//添加跟随上报，单独上报跟随上报状态字，增加3+11=14字节
			}
		}
		else
	    {
	    	if( DealFollowReportStatus(Ch, Buf+13) == TRUE )
	    	{
	    		Buf[2] = 2;//sequence of  A-ResultNormal	
            	DLT698AddBuf( Ch, 0x77, Buf, (13+11) );//添加跟随上报33200201、跟随上报状态字
            }
            else
            {
            	DLT698AddBuf( Ch, 0x77, Buf, 13 );//添加跟随上报33200201
            }	
	    }
	}
	else
	{
       DLT698AddOneBuf(Ch,0x77, 00);//添加00 无主动上报
	}    
}

//--------------------------------------------------
//功能描述:    处理时间标签
//         
//参数:      BYTE Ch[in]  通道选择
//         
//返回值:     无 
//         
//备注内容:
//--------------------------------------------------
void DealTimeTag( BYTE Ch)
{
    if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return;
    }
    
	if( APPData[Ch].TimeTagFlag != eNO_TimeTag )//如果有时间标签
	{
		DLT698AddOneBuf( Ch,0x77,1);//OPTIONAL 置为1
		DLT698AddBuf( Ch, 0x77, g_TimeTagBuf, 10 );//返回客户机下发的时间按标签
	}
	else
	{
		DLT698AddOneBuf( Ch,0x77,0);//添加时间标签
	}
}
//--------------------------------------------------
//功能描述:    处理时间标签与跟随上报函数
//         
//参数:      BYTE Ch[in]  通道选择
//         
//返回值:     无   
//         
//备注内容:    无
//--------------------------------------------------
void ProcFollowReportAndTimeFlag( BYTE Ch)//跟随上报信息域（FollowReport）  OPTIONAL
{	 
	DealFollowReport( Ch );
	DealTimeTag( Ch);//添加时间标签
}

//--------------------------------------------------
//功能描述:    应用层apdu处理函数
//         
//参数:      BYTE Ch[in]  通道选择
//         
//返回值:     无   
//         
//备注内容:    无
//--------------------------------------------------
void ProcDlt698Apdu( BYTE Ch)
{
    if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return;
    }
    
	if( APPData[Ch].eSafeMode != eSECURITY_FAIL )//解密失败不进行处理
	{
		switch( LinkData[Ch].pAPP[0] ) //cmd
		{
		case CONNECT_REQUEST:
			DealConnect_Request( Ch,&LinkData[Ch].pAPP[0],&APPData[Ch].pAPPBuf[0]);
			break;
		case RELEASE_REQUEST:
			DealRelease_Request(Ch);
			break;
		case GET_REQUEST:
			DealGet_Request( Ch,&LinkData[Ch].pAPP[0] );
			break;	
		case SET_REQUEST:
			DealSet_Request( Ch,&LinkData[Ch].pAPP[0] );
			break;	
		case ACTION_REQUEST:
			DealAction_Request( Ch,&LinkData[Ch].pAPP[0] );
			break;	
		case REPORT_RESPONSE:
			DealReportResponse_Request( Ch, &LinkData[Ch].pAPP[0] );
			break;
		// case PROXY_REQUEST:
		// 	DealProxy_Request( Ch, &LinkData[Ch].pAPP[0] );
		// 	break;
		case SECURITY_REQUEST:
			APPData[Ch].APPFlag = APP_ERR_DATA;
			break;		
		default:
			APPData[Ch].APPFlag = APP_ERR_DATA;
			break;
		}
		if( (APPData[Ch].APPFlag&APP_ERR_DATA) == APP_ERR_DATA )
		{
		    APPData[Ch].APPFlag = APP_HAVE_DATA;
		    APPData[Ch].APPBufLen.w = 0;
		    APPData[Ch].APPCurrentBufLen.w = APPData[Ch].APPBufLen.w;
            DLT698AddOneBuf( Ch, 0, ERROR_RESPONSE);
            DLT698AddOneBuf( Ch, 0, LinkData[Ch].pAPP[1]);
            DLT698AddOneBuf( Ch, 0x55, 1);//apdu无法解析
		}
		else
		{
            APPData[Ch].pAPPBuf[0] |= 0x80;//将请求置成响应
		}
	}	
}
//--------------------------------------------------
//功能描述:    组帧ERROW Response应答
//         
//参数:      BYTE Ch[in]  通道选择
//         
//返回值:     无 
//         
//备注内容:    无
//--------------------------------------------------
static void DealErrowResponse( BYTE Ch )
{
    if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return;
    }
    
    APPData[Ch].APPFlag = APP_HAVE_DATA;
    APPData[Ch].APPBufLen.w = 0;
    APPData[Ch].APPCurrentBufLen.w = APPData[Ch].APPBufLen.w;
    DLT698AddOneBuf( Ch, 0, ERROR_RESPONSE);
    DLT698AddOneBuf( Ch, 0, LinkData[Ch].pAPP[1]);
    DLT698AddOneBuf( Ch, 0x55, 1);//apdu无法解析
}
//--------------------------------------------------
//功能描述:    对应用层结构等数据进行初始化函数
//         
//参数:      BYTE Ch[in]  通道选择
//         
//返回值:     无 
//         
//备注内容:    无
//--------------------------------------------------
void InitDlt698APP( BYTE Ch)
{
    BYTE BufIndex;//应用层帧buf 需要提前考虑的数据长度
    
	if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return;
    }

    BufIndex = (LinkData[Ch].AddressLen+11);//初始化

    APPData[Ch].RequestType = LinkData[Ch].pAPP[0];//保存请求模式

    if( APPData[Ch].eSafeMode != eNO_SECURITY )//如果是安全传输
    {
    	if( (APPData[Ch].eSafeMode == eEXPRESS_MAC) ||(APPData[Ch].eSafeMode == eSECURITY_RN) )//如果是安全传输
    	{
    		BufIndex +=12;
    	}
    	else if( APPData[Ch].eSafeMode == eSECRET_MAC )
    	{
    		BufIndex +=(12+15);//安全传输数据 + 密文可能出现的最多补齐数
    	}
    	else if( APPData[Ch].eSafeMode == eSECRET_TEXT )
    	{
    		BufIndex += 15;//安全传输数据 + 密文可能出现的最多补齐数
    	}
    }

    if( APPData[Ch].TimeTagFlag != eNO_TimeTag )//如果存在时间标签预留13字节的数据标签长度
    {
    	BufIndex += 13;
    }
    
    BufIndex += 2;//num个数预留空间
    if( APDUBufFlag.Request.Ch == Ch )
    {
        APDUBufFlag.Request.Flag = FALSE;//到达应用层 说明链路层分帧已结束
    }
    
    APPData[Ch].pAPPBuf = AllocResponseBuf( Ch, ePROTOCO_698 );//初始化应用层buf指针
    APPData[Ch].APPMaxBufLen.w = (APP_APDU_BUF_SIZE - BufIndex);
}
//--------------------------------------------------
//功能描述:    应用层数据处理函数
//         
//参数:      BYTE Ch[in]   通道选择
//         
//返回值:     无 
//         
//备注内容:    无
//--------------------------------------------------
static void ProcDlt698App( BYTE Ch )
{ 
	BYTE i = 0;
	BOOL Result = FALSE;
	
	if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return;
    }
    
 	if( LinkData[Ch].eLinkDataFlag == eAPP_OK )//链路层数据准备好
 	{
 	    memset( &APPData[Ch].APPFlag, 0x00, sizeof(TDLT698APPData));//初始化应用层结构体
 	    	
        if( (api_GetSysStatus( eSYS_STATUS_ID_698MASTER_AUTH ) == TRUE)
        || (api_GetSysStatus( eSYS_STATUS_ID_698TERMINAL_AUTH ) == TRUE))//如果进行已建立应用链接通讯，刷新建立应用链接时间
        {
        	for( i = 0; i < eConnectModeMaxNum; i++ )
			{
				if( i == eConnectGeneral )//一般密码不维持心跳
				{
					continue;
				}
				
				if( APPConnect.ConnectInfo[i].ConnectValidTime != 0 )
				{
					APPConnect.ConnectInfo[i].ConnectValidTime = APPConnect.ConnectInfo[i].ConstConnectValidTime;
				}
			}
        }
        
		#if(SEL_ESAM_TYPE != 0)
 		APPData[Ch].eSafeMode = ProcSecurityRequestData( Ch );//先进行安全传输判断
 		#else
		APPData[Ch].eSafeMode = eNO_SECURITY;
		#endif

	 	Result = JudgeTimeTagValid( Ch );//解密数据后进行时间标签判断 加密数据中包含时间标签

	 	InitDlt698APP(Ch);//对应用层结构体等进行初始化
//		if(Result == TRUE)
//		{
//			if( JudgeChargingPilePro(Ch) == FALSE)
//			{
//				api_InitSci( Ch );
//				return;
//			}
//		}

 		if( Result == FALSE )//解析apdu出现错误
 		{
            DealErrowResponse( Ch );
            
            ProcFollowReportAndTimeFlag( Ch);//添加时间标签和主动上报
            
            ProcSecurityResponseData( Ch);//安全返回处理
    
            ProFollowJudge(Ch);//链路层分帧判断
 		}
 		else
 		{            
            ProcDlt698Apdu(Ch);//进行apdu的处理
            
            ProcFollowReportAndTimeFlag( Ch);//添加时间标签和主动上报
            
            ProcSecurityResponseData( Ch);//安全返回处理
    
            ProFollowJudge(Ch);//链路层分帧判断
 		}
 	}
}

#endif//(SEL_DLT698_2016_FUNC == YES )
