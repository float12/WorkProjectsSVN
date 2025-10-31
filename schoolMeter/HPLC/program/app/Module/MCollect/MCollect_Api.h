//----------------------------------------------------------------------
//Copyright (C) 2003-2021 烟台东方威思顿电气股份有限公司电表软件研发部
//创建人	
//创建日期	
//描述		
//修改记录--20250415 lsc 删除不必要函数声明
//----------------------------------------------------------------------

#ifndef __MCOLLECT_API_H
#define __MCOLLECT_API_H
//--------------------------------------

//-----------------------------------------------
//				宏定义
//-----------------------------------------------
#define  IDF_RES_NUM			10 		//电器数量
#define  IDF_LOOP_NUM			2 		//第几回路
#define  MAX_SEND_DELAY   	 (150)    	//1.5秒
#define  MAX_TX_COMMIDLETIME (80)		//800毫秒
#define  MAX_RX_COMMIDLETIME (100)		//1秒   

#define ARRAY_SZ(X) (sizeof((X)) / sizeof((X)[0]))
/**********************************************
*brief 485-2 采集标识
***********************************************/
#define		MSF_AP			0			//正向有功电能（带费率）
#define     MSF_AR          1
#define     MSF_RP          2                   
#define     MSF_RR          3
#define     MSF_PA          4
/**********************************************
*brief 本体 采集标识
***********************************************/
#define 	MSF_VOL_CUR_PARA	5			//电压电流系数
#define 	MSF_IDENTIFY_PARA	6			//识别结果
#define 	MSF_TIME_PARA		7			//时间

//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
typedef enum
{
	eVolTime = 0,  // 传输电压和时间
	eAjust,
	eInit
} eF460SpiCommCmd;
#pragma pack(1)
typedef struct TElectricityEnergy_t
{
	BYTE Time;		//时间
	DWORD Energy;	//电能
}TElectricityEnergy;

typedef struct TElectricityCommonData_t
{
	BYTE ADRR[7];									//本体地址
	DWORD Power;									//功率之和
	TElectricityEnergy ElectricitySwtichEnergy;		//开关电能
	TElectricityEnergy ElectricityCombEnergy;		//组合电能
	BYTE MeterNum;									//抄表个数
	BYTE MeterAdrr[8];							//抄表地址-支持20台表
}TElectricityExtData;
#pragma pack()


//过流参数
typedef struct TOverIPara_t
{
	BYTE 		Warn;		//告警投入
	BYTE 		Out;		//出口投入
	SDWORD 		Value;		//定值
	DWORD 		Time;		//时间
	DWORD 		CRC32;		//校验
} TOverIPara;

typedef struct TUnitSParaSafeRom_t
{
	TOverIPara 	OverIPara[4];		//过流参数，每项含有4字节CRC
	BYTE EquipmCtrlWord[8+4];		//8字节数据 + 4字节CRC
}TUnitSParaSafeRom;

#pragma pack(1)
typedef struct TElectIdentify_t
{
	BYTE  ucMessageID;	//第x个结果
	BYTE  ucCode;		//电器代码
	BYTE  ucdevNum;		//电器设备数
	SWORD ucPower;		//电器功率
}TElectIdentify;
typedef struct
{
	unsigned short	ApplClassID;	//设备类号
	unsigned char 	ApplStatus;		//设备状态
	unsigned long 	ApplP;			//设备有功功率
}T_ApplCurData;
typedef struct
{
	unsigned short	ApplClassID;	//设备类号
	unsigned long 	ApplFrzCylP;	//冻结周期内有功用电量
	unsigned long 	ApplFrzCylAvgP;	//冻结周期内设备运行平均功率
	unsigned char	StartTime;		//启动时间
	unsigned char	EndTime;		//停止时间
	unsigned char	SECount;		//合并启停数
}T_ApplFrzData;

typedef struct AMessage
{
	TRealTimer ucTime;				//时间
	BYTE  ucMessageID;				//结果个数
	T_ApplFrzData ucData[10];		//电器数据
}AMessage_T;
//测试采集任务收发报文的次数
typedef struct TCollectSendRec_t
{
	DWORD Sec_Send_Success;
	DWORD Sec_Rec_Success;
	DWORD Sec_Rec_Failure;
	DWORD Min_Send_Success;
	DWORD Min_Rec_Success;
	DWORD Min_Rec_Failure;
}TCollectSendRec;
#pragma pack()

typedef enum TRANS_DIR{ TD12, TD21 } TRANS_DIR; //转换方向: TD12:1形参->2形参; TD21:2->1

typedef struct TTriggerViolationSign_t
{
	BYTE  ReasonFlag;	//触发原因标志
	BYTE  DurationTime;	//标志触发持续时间
}TTriggerViolationSign;
#if(SEL_F415 == YES)
//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
extern BYTE Address_645[6];							//415回传的地址域，同6025通信地址一致
//TSendRec目前的作用只是监测两路uart
extern TCollectSendRec TSendRec[2];
extern BYTE ControlResetStatus;						//是否允许415复位，0：默认允许；1：不允许415复位

extern AMessage_T MessageData;						//415返回的识别结果--主要为电动车识别结果
//-----------------------------------------------
//				函数声明
//-----------------------------------------------
//-----------------------------------------------
//函数功能: 检查端口是否处于级联状态
//
//参数: 	无
//			
//返回值:  	BOOL
//
//备注:   
//-----------------------------------------------
BOOL IsJLing(BYTE Ch, BYTE *pGyType,BYTE *pSourceCh);
//-----------------------------------------------
//函数功能: 往级联端口写数据
//
//参数: 	无
//			
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void JLWrite(BYTE destCh,BYTE *hBuf,DWORD dwLen,BYTE byGyType,BYTE sourceCh);
//-----------------------------------------------
//函数功能: 检查是上行端口收到的报文，是否需要转发处理
//
//参数: 	sourchCh-源端口，pDestCh-目标端口
//			
//返回值:  	BOOL
//
//备注:   
//-----------------------------------------------
BOOL IsNeedTransmit(BYTE sourceCh,BYTE byGy, BYTE *pBuf,BYTE *pDestCh);
BOOL IsGyTransDI(DWORD DI);
BOOL GyRxMonitor(BYTE Ch,BYTE GyType);
void api_PowerUpCollect(void);
void  api_ReadIdentResult(BYTE i, BYTE* Buf );
//--------------------------------------------------
//功能描述:  置标志
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void SetCJFlag_m(BYTE Ch,BYTE Flag);
//--------------------------------------------------
//功能描述: 6025_to_415采集任务状态的读取
//
//参数:			0:采集状态读取
//				1:采集状态设置
//
//返回值:		BYTE
//
//备注:415升级时，采集任务关闭；415执行升级时，采集任务打开
//			IsUpdateF415		采集任务状态
//				FALSE  			  1--开启
//				TRUE  			  0--关闭
//--------------------------------------------------
BYTE api_GetCollectionTaskStatus415(void);
//--------------------------------------------------
//功能描述: 6025_to_415采集任务状态的设置
//
//参数:			TYPE : 0——————采集状态关闭----415升级任务允许执行
//				TYPE : 1——————采集状态开启----415升级任务拒绝执行
//
//返回值:		BYTE
//
//备注:
//--------------------------------------------------
void api_SetCollectionTaskStatus415(BYTE TYPE);
//-----------------------------------------------
//函数功能: 采集大循环任务
//
//参数:		无 
//                 
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
void api_MCollectTask(void);
#endif//#if(SEL_F415 == YES)
#endif // __MCOLLECT_API_H