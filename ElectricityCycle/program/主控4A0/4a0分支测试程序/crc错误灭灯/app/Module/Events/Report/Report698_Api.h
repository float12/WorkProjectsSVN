//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	魏灵坤 张玉猛
//创建日期	2016.10.26
//描述		698规约上报
//修改记录	
//---------------------------------------------------------------------- 

#ifndef __REPORT698_API_H
#define __REPORT698_API_H

//-----------------------------------------------
//				宏定义
//-----------------------------------------------

//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------

//跟随上报状态字中各个状态位，定义顺序考虑698.45协议特点高字节在前
typedef enum
{
	eReserve0 = 0,			//最高字节的bit0，即协议中跟随上报状态字的bit24
	eReserve1,
	eReserve2,
	eReserve3,	
	eReserve4,
	eReserve5,
	eReserve6,	
	eReserve7,
	eReserve8,			//第3高字节的bit0，即协议中跟随上报状态字的bit16
	eReserve9,
	eReserve10,
	eReserve11,	
	eReserve12,
	eReserve13,
	eReserve14,	
	eReserve15,	
	eSTATUS_ReadMeterBatteryUnderVol,   //第2高字节的bit0，即协议中跟随上报状态字的bit8
	eSTATUS_TickFlag,					//第2高字节的bit1，即协议中跟随上报状态字的bit9--透支状态 TickFlag
	eReserve18,
	eReserve19,
	eReserve20,
	eReserve21,
	eSTATUS_OpenRelaySucceed,
	eSTATUS_CloseRelaySucceed,		
	eReserve24,							//第1高字节的bit0，即协议中跟随上报状态字的bit0
	eSTATUS_ESAM_Error,					//第1高字节的bit1，即协议中跟随上报状态字的bit0
	eReserve26,
	eSTATUS_ClockBatteryUnderVol,
	eReserve28,
	eSTATUS_EEPROM_Error,//bit5--存储器故障或损坏
	eReserve30,
	eReserve31,
	eMAX_FOLLOW_REPORT_STATUS_NUM,
}eFollowReportStatus;

typedef enum
{
	eFollowReport = 0,
	eActiveReport,
	eFollowStatusReportMethod,
}eReportType;

typedef enum
{
	eReportMode = 0,	//上报模式
	eReportMethod,		//上报方式 ---固定最后一个--jwh
}eReportClass;

typedef enum
{
	eGetReportAllList = 0,		//全部新增列表
	eGetReportActiveList,		//主动上报新增列表
}eGetReportListType;

typedef enum
{
	eActiveReportPowerDown = 0,	//上报掉电
	eActiveReportNormal,		//正常上报	
}eActiveReportMode;

#pragma pack(1)

typedef struct TReportMode_t
{
	//4300	电气设备 的属性7. 允许跟随上报
	BOOL 	bMeterReportFlag;
	//4300	电气设备 的属性8，允许主动上报
	BOOL	bMeterActiveReportFlag;
	BYTE 	byFollowReportMode[4];//20150400 电能表跟随上报模式字
	//事件上报标识：不上报（0），事件发生上报（1），事件恢复上报（2），事件发生恢复均上报（3）
	BYTE 	byReportMode[eNUM_OF_EVENT_PRG];
	//事件上报方式 0：主动上报 1：跟随上报
	BYTE 	byReportMethod[(eNUM_OF_EVENT_PRG/8)+1];
	BYTE 	byReportStatusMethod;
	//4300	电气设备 的属性10	上报通道	array OAD
	DWORD 	ReportChannelOAD[MAX_COM_CHANNEL_NUM];
	DWORD	CRC32;
}TReportMode;

typedef struct TReportData_t
{
	BYTE Status[4];							//跟随上报状态字
	BYTE Index[SUB_EVENT_INDEX_MAX_NUM];	//新增事件列表
	DWORD CRC32;
}TReportData;

typedef struct TReportTimes_t
{
	BYTE Times;					//上报次数
	DWORD CRC32;
}TReportTimes;
#pragma pack()

typedef struct TReportSafeRom_t
{
	TReportMode		ReportMode;
	#if( SEL_DLT645_2007 == YES )
	TReport645ResetTime Report645ResetTime;
	TReport645 Report645;
	#endif
	TReportTimes ReportTimes;
}TReportSafeRom;

typedef struct TReport_t
{
   TReportData ReportData[MAX_COM_CHANNEL_NUM];//上报数据
	
}TReportConRom;
//-----------------------------------------------
//				变量声明
extern __no_init TReportData ReportData[MAX_COM_CHANNEL_NUM];//电能表跟随上报状态字
extern __no_init TReportData ReportDataBak[MAX_COM_CHANNEL_NUM];
extern WORD EventLostPowerEndTime; //掉电事件结束时间，累加（单位秒）


//-----------------------------------------------

//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
//-----------------------------------------------
//函数功能: 	主动上报时间减减
//
//参数: 		无
//        	
//返回值:		无
//			
//备注:主动上报时间处理，放在systick中断中，越减短越好，
//-----------------------------------------------
void ReportTimeMinus( );

//-----------------------------------------------
//函数功能: 设置新增事件列表
//
//参数: 
//		inSubEventIndex[in]:	时间索引号
//  	BeforeAfter[in]:		EVENT_START	BIT0	
//  	                		EVENT_END	BIT1
//返回值:	无
//			
//备注:3320的属性2，不是发生结束都具备的输入 EVENT_START+EVENT_END
//-----------------------------------------------
void SetReportIndex( BYTE inSubEventIndex, BYTE BeforeAfter );

//-----------------------------------------------
//函数功能: 清指定通道新增事件列表中的指定事件
//
//参数: 
//		Ch[in]:				通道号
//  	inSubEventIndex[in]:事件索引号	
//  	                	
//返回值:	无
//			
//备注:单通道
//-----------------------------------------------
void ClrReportIndexChannel( BYTE Ch, BYTE inSubEventIndex );
//-----------------------------------------------
//函数功能: 清所有通道新增事件列表中的指定事件
//
//参数: 
//  	inSubEventIndex[in]:事件索引号	
//  	                	
//返回值:	无
//			
//备注:所有通道
//-----------------------------------------------
void ClrReportAllChannelIndex(  BYTE inEventIndex );

//-----------------------------------------------
//函数功能: 清上报数据
//
//参数: 	无
//  	                	
//返回值:	无
//			
//备注:清所有通道的新增事件列表、指针
//-----------------------------------------------
void ClearReport( BYTE Type );

//-----------------------------------------------
//-----------------------------------------------
//函数功能: 获取跟随上报OAD
//
//参数: 	
//		Type[in]:	0--跟随上报调用	1--EVENTOUT使用
//		Ch[in]:		通道号
//		Buf[out]：	计算出的单个OAD 
//           	
//返回值:	TRUE--有跟随上报	FALSE--无跟随上报
//			
//备注:
//-----------------------------------------------
BOOL GetFirstReportOad(BYTE Type, BYTE Ch, BYTE *Buf);//函数功能: 获取新增事件列表OAD
//
//参数: 	
//		Len[in]:	输入数据长度
//		Buf[out]：	array OAD 
//           	
//返回值:	返回数据长度 0x8000--输入Buf长度不足
//			
//备注:新增事件列表 属性2
//-----------------------------------------------
WORD GetReportOadList( eGetReportListType GetReportListType, BYTE Ch, WORD Len, BYTE *Buf);

//-----------------------------------------------
//函数功能: 掉电事件上报组帧
//
//参数:	inSubEventIndex[in]: eSUB_EVENT_LOST_POWER（掉电事件）
//							其他(其他事件返回)
//
//		RecordNo[in]: 		记录序号，写到buf中，掉电发生上报用
//							
//返回值:  
//			
//备注: 
//-----------------------------------------------
void api_LostPowerReportFraming( BYTE inSubEventIndex ,DWORD RecordNo );

//-----------------------------------------------
//函数功能: 主动上报判断
//
//参数:	   无
//         
//返回值:     TRUE：产生主动上报         FLASE:无新增上报
//			
//备注: 
//-----------------------------------------------
WORD api_JudgeActiveReport( void );
//-----------------------------------------------
//函数功能: 确认主动上报
//
//参数:	   BYTE Type[in] 0x00: 确认新增事件列表 0x55：确认跟随上报状态字
//         
//			
//备注: 
//-----------------------------------------------
void api_ResponseActiveReport( BYTE Type );

//-----------------------------------------------
//函数功能: 备份上报列表
//
//参数: 	
//           	
//返回值:	
//			
//备注:
//-----------------------------------------------
void api_BackupReportIndex( void );

//-----------------------------------------------
//函数功能: 获取需上报事件对象列表 OI
//
//参数: 	
//		Len[in]:	输入数据长度
//		Buf[out]：	array OI 
//           	
//返回值:	返回数据长度 0x8000--输入Buf长度不足
//			
//备注:新增事件列表 属性3 需上报事件对象列表
//-----------------------------------------------
WORD GetReportOIList(WORD Len, BYTE *Buf);

//-----------------------------------------------
//函数功能: 上报电表初始化
//
//参数: 	无	
//           	
//返回值:	无
//			
//备注:
//-----------------------------------------------
void FactoryInitReport(void);

//-----------------------------------------------
//函数功能: 设置上报通道
//
//参数: 	
//		byIndex[in]:	通道索引，对应ReportChannelOAD[0]数组下标，0--所有上报通道 1--第1个上报通道
//		Num[in]:		设置几个上报通道,如果byIndex不为0，则只能设置1个上报通道
//		pBuf[in]:		array OAD
//       	
//返回值:	TRUE/FALSE
//			
//备注:4300电气设备的属性10 上报通道array OAD
//-----------------------------------------------
BOOL SetReportChannel( BYTE byIndex, BYTE Num, BYTE *pBuf );
//-----------------------------------------------
//函数功能: 获取跟随上报通道标志
//
//参数: 	   Ch[in]:		通道号
//           	
//返回值:	   TRUE-此通道支持上报	FALSE--此通道不支持上报
//			
//备注:
//-----------------------------------------------
BOOL api_GetReportChannelStatus( BYTE Ch );
//-----------------------------------------------
//函数功能: 读取上报通道
//
//参数: 		
//			pBuf[in]:		array OAD
//       	
//返回值:	数据长度
//			
//备注:4300电气设备的属性10 上报通道array OAD
//-----------------------------------------------
WORD ReadReportChannel( BYTE *pBuf );


//-----------------------------------------------
//函数功能: 设置允许上报标志
//
//参数: 
//  		bMeterReportFlag[in]:	TRUE/FALSE
//			Type					eFollowReport/eActiveReport
//返回值:	TRUE/FALSE
//			
//备注:4300电气设备 属性7.8 允许跟随上报,允许主动上报
//-----------------------------------------------

BOOL SetReportFlag( BYTE bMeterReportFlag,eReportType Type );

//-----------------------------------------------
//函数功能: 读取允许上报标志
//
//参数: 	Type		eFollowReport/eActiveReport
//
//返回值:	TRUE--允许上报	FALSE--不允许上报
//			
//备注:4300电气设备 属性7.8 允许跟随上报,允许主动上报
//-----------------------------------------------

BOOL ReadReportFlag( eReportType Type );

//-----------------------------------------------
//函数功能: 设置事件上报标识
//
//参数: 	
//			eReportClass ReportClass
//			OI[in]:				事件OI
//			byReportMode[in]:	eReportMode 	事件上报标识{不上报(0),事件发生上报(1),事件恢复上报(2),事件发生恢复均上报(3)}
//								eReportMethod 	0:主动上报 1：跟随上报
//
//返回值:	TRUE/FALSE
//			
//备注:接口类7的属性8，接口类24的属性11
//-----------------------------------------------
BOOL api_SetEventReportMode( eReportClass ReportClass, WORD OI, BYTE byReportMode );

//-----------------------------------------------
//函数功能: 读取事件上报标识
//
//参数: 	
//			eReportClass ReportClass
//			OI[in]:				事件OI
//			byReportMode[in]:	eReportMode 	事件上报标识{不上报(0),事件发生上报(1),事件恢复上报(2),事件发生恢复均上报(3)}
//								eReportMethod 	0:主动上报 1：跟随上报
//
//返回值:	TRUE/FALSE
//			
//备注:接口类7的属性8，接口类24的属性11
//-----------------------------------------------
BOOL api_ReadEventReportMode( eReportClass ReportClass, WORD OI, BYTE *pBuf );


//-----------------------------------------------
//函数功能: 置位跟随上报状态字对位的状态位
//
//参数: 
//			StatusNo[in]		系统状态号
//                    
//返回值:  	无
//
//备注: 只有在该状态位由0变1时才调用本函数  
//-----------------------------------------------
void api_SetFollowReportStatus(BYTE StatusNo);
//-----------------------------------------------
//函数功能: 读取跟随上报状态字、模式字
//
//参数:		BYTE Ch[in]  通道选择
//			BYTE Mode[in]：	   模式 0： 代表读取电表状态字           1：代表读电表模式字 2:返回数据判断跟随上报标识-用于跟随上报
//          BYTE *pBuf[in]：    输入buf      
//         
//返回值:    返回变量长度 0x0000:数据长度不够  0x8000: 出现错误
//			
//备注:
//-----------------------------------------------
WORD api_ReadFollowReportStatus_Mode( BYTE Ch, BYTE Mode, BYTE *pBuf );

//-----------------------------------------------
//函数功能: 上报小时任务
//
//参数: 	无
//
//返回值:	无
//			
//备注:
//-----------------------------------------------
void ReportHourTask(void);

//-----------------------------------------------
//函数功能: 设置跟随上报模式字
//
//参数:    
//			pBuf[in]：	保存数据的缓冲
//返回值:	TRUE/FALSE  
//			
//备注: 
//-----------------------------------------------
WORD api_SetFollowReportMode( BYTE *pBuf );
//-----------------------------------------------
//函数功能: 上报秒任务
//
//参数: 	无
//
//返回值:	无
//			
//备注:
//-----------------------------------------------
void ReportSecTask(void);

//-----------------------------------------------
//函数功能: 上报上电任务
//
//参数: 	无
//
//返回值:	无
//			
//备注:
//-----------------------------------------------		
void PowerUpReport(void);

//-----------------------------------------------
//函数功能: 上报掉电任务
//
//参数: 	无
//
//返回值:	无
//			
//备注:
//-----------------------------------------------	
void PowerDownReport(void);
//-----------------------------------------------
//函数功能: 掉电主动上报发送
//
//参数:	 
//         
//返回值:   
//			
//备注: 低功耗大循环调用
//-----------------------------------------------
void ProcLowPowerMeterReport( void);


#endif//对应文件最前面的 #define __REPORT698_API_H
