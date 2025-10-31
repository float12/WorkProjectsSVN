//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.9.10
//描述		时间方面相关
//修改记录	
//----------------------------------------------------------------------
#ifndef __RTC_API_H
#define __RTC_API_H

//-----------------------------------------------
//				宏定义
//-----------------------------------------------
#define MAX_VALID_MONTH		12
#define MAX_VALID_DAY		31
#define MAX_VALID_HOUR		23
#define MAX_VALID_MINUTE	59


#define DATETIME_SECOND_FIRST			BIT15//秒在buf[0]
#define DATETIME_BCD					BIT14//读出数据是bcd
#define DATETIME_20YY					(BIT7+BIT6)
#define DATETIME_YY						BIT6
#define DATETIME_MM						BIT5
#define DATETIME_DD						BIT4
#define DATETIME_WW						BIT3
#define DATETIME_hh						BIT2
#define DATETIME_mm						BIT1
#define DATETIME_ss						BIT0

#define DATETIME_YYMMDD					(BIT6+BIT5+BIT4)
#define DATETIME_20YYMMDD				(BIT7+BIT6+BIT5+BIT4)
#define DATETIME_YYMMDDWW				(BIT6+BIT5+BIT4+BIT3)
#define DATETIME_20YYMMDDWW				(BIT7+BIT6+BIT5+BIT4+BIT3)
#define DATETIME_hhmmss					(BIT2+BIT1+BIT0)

#define DATETIME_MMDDhhmm				(BIT5+BIT4+BIT2+BIT1)
#define DATETIME_YYMMDDhhmm				(BIT6+BIT5+BIT4+BIT2+BIT1)
#define DATETIME_YYMMDDWWhhmm			(BIT6+BIT5+BIT4+BIT3+BIT2+BIT1)
#define DATETIME_YYMMDDWWhhmmss			(BIT6+BIT5+BIT4+BIT3+BIT2+BIT1+BIT0)
#define DATETIME_20YYMMDDhhmm			(BIT7+BIT6+BIT5+BIT4+BIT2+BIT1)
#define DATETIME_MMDDhhmmss				(BIT5+BIT4+BIT2+BIT1+BIT0)
#define DATETIME_YYMMDDhhmmss			(BIT6+BIT5+BIT4+BIT2+BIT1+BIT0)
#define DATETIME_20YYMMDDhhmmss			(BIT7+BIT6+BIT5+BIT4+BIT2+BIT1+BIT0)

//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
#pragma pack(1)
//定义实时时钟结构 698协议中 date_time_s 就是这种结构 要求年是word，但长度为7个字节
typedef struct TRealTimer_t
{
	//年
	WORD wYear;
	//月
	BYTE Mon;
	//日
	BYTE Day;
	//时
	BYTE Hour;
	//分
	BYTE Min;
	//秒
	BYTE Sec;

	char Timezone;
}TRealTimer;


typedef struct TRealTimerDate_t
{
	//年
	WORD wYear;
	//月
	BYTE Mon;
	//日
	BYTE Day;
	//星期
	BYTE Week;
	//时
	BYTE Hour;
	//分
	BYTE Min;
	//秒
	BYTE Sec;
	// ms
	WORD Ms;

}TRealTimerDate;

#pragma pack()


typedef struct TRtcTemperature_t
{
	BYTE Temperature[6];
	DWORD CRC32;
}TRtcTemperature;

typedef struct TRtcSafeRom_t
{
	TRtcTemperature RtcTemperature;
	BYTE Reserve[40];
}TRtcSafeRom;


//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern TRealTimer RealTimer;
//  系统全局变量 电表中当前时间与2000年1月1日0时0分之间的时间差，单位：分钟
extern DWORD g_RelativeMin;
extern BYTE g_PowerOnSecTimer;//只有上电大于30秒，掉电时才数据转存

//-----------------------------------------------
// 				函数声明
//-----------------------------------------------

//--------------------------------------------------
//功能描述:  从645的年月日时分转换到 TRealTiemr
//         
//参数:    pBuf[in]:645的年月日时分,传输顺序：分 时 日 月 年
//         
//		   pRealTimer[out]:转换后得到的 TRealTimer		
//
//返回值:   得到的DWORD 
//         
//备注内容:  无
//--------------------------------------------------	
void api_ConvertYYMMDDhhmm_TRealTimer( TRealTimer *pRealTimer, BYTE *pBuf );

//-----------------------------------------------
//函数功能: 计算两个绝对时间之间的分钟数
//
//参数: 
//			tNew[in]			绝对时间的时间格式，值必须是Hex码
//			tOld[in]			绝对时间的时间格式，值必须是Hex码
//                    
//返回值:  	两个绝对时间之间的分钟数 tNew - tOld
//			0xffffffff: 时间有不合法的情况
//			0:          tNew < tOld
//备注:     时间都是Hex码
//-----------------------------------------------
DWORD api_CalcIntervalMinute(TRealTimer * tNew, TRealTimer * tOld);


//-----------------------------------------------
//函数功能: 计算相对时间的分钟数，以2000年1月1日0点0分为起点
//
//参数: 
//			t[in]			绝对时间的时间格式，值必须是Hex码
//                    
//返回值:  	相对时间值
//			0xffffffff  时间非法
//备注:     时间都是Hex码
//-----------------------------------------------
DWORD api_CalcInTimeRelativeMin(TRealTimer * t);

//--------------------------------------------------
//功能描述:  获取时间戳
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
QWORD  getmktimems( TRealTimer * t );

//-----------------------------------------------
//函数功能: 计算相对时间的秒数
//
//参数: 
//			t[in]			绝对时间的时间格式，值必须是Hex码
//                    
//返回值:  	相对时间的秒数
//			0xffffffff  时间非法
//备注:     时间都是Hex码
//-----------------------------------------------
QWORD api_CalcInTimeRelativeSec(TRealTimer * t);


//-----------------------------------------------
//函数功能: 读取时间，调用该函数会操作外部时钟，同时刷新表示时间变量，不要频繁操作
//
//参数: 
//			Type[in]			0：读出rtc模块时钟，刷新RealTimer变量 1：只读秒
//                    
//返回值:  	TRUE:正确读取       FALSE:没有正确读取
//备注:     
//-----------------------------------------------
BOOL api_ReadMeterTime( BYTE Type );


//-----------------------------------------------
//函数功能: 把TRealTimer结构的时间写到RTC模块中，星期由日期计算
//
//参数: 
//			t[in]				要写入的时间
//                    
//返回值:  	TRUE:正确读取       FALSE:没有正确读取
//备注:     
//-----------------------------------------------
BOOL api_WriteMeterTime(TRealTimer * t);


//-----------------------------------------------
//函数功能: 根据入口参数读取时间
//
//参数: 
//			Type[in]				需要获取的时间根式
//									bit15: 0：年月日星期时分秒 年在buf[0]且年是小端模式   1：秒分时星期日月年 年是小端模式
//									bit14: 0: Hex码  	1:BCD码
//									bit13-bit8:保留
//									bit7:年是否加2000  1：加2000
//									bit6-bit0:年月日星期时分秒  1:表示需要该位对应的数据
//			pBuf[out]				存放读出的数据
//                    
//返回值:  	返回读取的数据长度
//备注:     时间都是Hex码
//-----------------------------------------------
WORD api_GetRtcDateTime( WORD Type, BYTE *pBuf );


//-----------------------------------------------
//函数功能: 判断时钟有效性的函数
//
//参数: 
//			p[in]				TRealTimer结构的绝对时间，值必须是Hex码
//                    
//返回值:  	FALSE--数据非法 TRUE--数据合法
//备注:     时间都是Hex码
//-----------------------------------------------
BOOL api_CheckClock(TRealTimer * p);

//---------------------------------------------------------------
//函数功能: BCD格式的时间合法性判断
//
//参数: 	
//			Start[in]  开始判断的地址
// 			Ck[in]     传入的时间 不定长 格式：年、月、日、周、时、分
// 			Len[in]	   比较长度 最高位为1表示没有周
//                   
//返回值:  
//
//备注:   
//---------------------------------------------------------------
BYTE api_CheckClockBCD( BYTE Start, BYTE Len, BYTE *Ck );//周在中间
#if( PREPAY_MODE == PREPAY_LOCAL )
//---------------------------------------------------------------
//函数功能: 判断阶梯结算日合法性
//
//参数: 	Buf[in]  传入格式  [0]时[1]日[2]月			
// 			                  
//返回值:  true 至少有一个结算日有效 false 所有结算日都无效
//
//备注:   
//---------------------------------------------------------------
BYTE api_JudgeYearClock( BYTE *Buf );
#endif
//--------------------------------------------------
//功能描述:  645 时间格式的判断以及与当前时间的比较
//         
//参数:      BYTE* ProBuf[in]
//         
//返回值:    
//         
//备注内容:  无
//--------------------------------------------------
BOOL api_JudgeClock645( BYTE* ProBuf);

void api_RtcTask(void);
void api_PowerUpRtc( void );




//-----------------------------------------------
//函数功能: 相对时间转化为绝对时间，相对时间指以分钟为单位，以2000年1月1日0点0分为起点，100年内有效
//
//参数: 
//			Min[in]				相对时间的分钟数
//			p[out]				返回绝对时间（年月日时分秒格式）
//                    
//返回值:  	TRUE:正确转换       FALSE:没有正确转换
//备注:     时间都是Hex码
//-----------------------------------------------
BOOL api_ToAbsTime(DWORD Min, TRealTimer *p);

//-----------------------------------------------
//函数功能: 相对时间转化为绝对时间（645格式），相对时间指以分钟为单位，以00年1月1日0点0分为起点，100年内有效
//
//参数: 
//			Min[in]				相对时间的分钟数
//			p[out]				返回绝对时间（分时日月年格式）
//                    
//返回值:  	TRUE:正确转换       FALSE:没有正确转换
//备注:     时间都是BCD码，年在后，为一个字节
//-----------------------------------------------
BOOL api_ToAbsTime645(DWORD Min, BYTE *p);

//-----------------------------------------------
//函数功能: RTC误差校准
//
//参数:		err[in]   秒脉冲误差(每天多少秒，单位0.001)  最高位为1表示负值
//						
//返回值:	TRUE/FALSE
//		   
//备注:
//-----------------------------------------------
BOOL api_CaliRtcError( WORD err );

//初始化秒脉冲输出方式
//Type -- 0			1HZ脉冲输出
//Type -- 不为零	禁止秒脉冲输出
void api_InitOutClockPuls(BYTE Type);

//-----------------------------------------------
//函数功能: 时钟补偿电表初始化
//
//参数: 	无
//                    
//返回值:  	无
//
//备注: 清补偿系数  
//-----------------------------------------------
void api_FactoryInitRtc(void);

//-----------------------------------------------
//函数功能: 检查相应年份的月、日数据合法性，输入参数和 RealTimer 中数据一样是Hex码
//
//参数: 
//			wYear[in]			年，值必须是Hex码
//			byMon[in]			月，值必须是Hex码
//			byDay[in]			日，值必须是Hex码
//                    
//返回值:  	FALSE--数据非法 TRUE--数据合法
//备注:     时间都是Hex码
//-----------------------------------------------
BOOL api_CheckMonDay( WORD wYear, BYTE byMon, BYTE byDay );

//-----------------------------------------------
//函数功能: 规约读写rtc调校系数
//
//参数: 	Type[in]	READ/WRITE
//			Buf[in/out] 缓冲                  
//返回值:  	Buf长度
//
//备注:   
//-----------------------------------------------
BYTE api_ProcRtcPara(BYTE Type, BYTE *Buf);

//---------------------------------------------------------------
//函数功能: 读写InfoFlash中RTC的A、B、C、D、E校正系数
//
//参数: 	pBuff: 校正系数数据
//                   
//返回值:  	pBuff长度
//
//备注:   
//---------------------------------------------------------------
void ProcInfoRtcAdjustPara( BYTE Type, BYTE *pBuff );

//---------------------------------------------------------------
//函数功能: 写InfoFlash中RTC校正参数错误校验和
//
//参数: 	无
//                   
//返回值:  	无
//
//备注:   校验和错误会采用默认参数, 不使用InfoFlash中的参数
//---------------------------------------------------------------
void WriteInfoRTCErrorCheckSum( void );

//-----------------------------------------------
//函数功能: 将698时间格式转换为645格式
//
//参数: 
//			pTime698[in]				698时间格式 HEX 年为两字节，年在前
//			pTime645[out]				645时间格式 BCD 年为一字节，年在前
//                    
//返回值:  	无
//备注:     
//-----------------------------------------------
void api_TimeFormat698To645(TRealTimer *pTime698, BYTE *pTime645);

//-----------------------------------------------
//函数功能: 将698时间格式转换为645格式
//
//参数: 
//			pTime645[in]				645时间格式 BCD 年为一字节，年在前
//			pTime698[out]				698时间格式 HEX 年为两字节，年在前
//                    
//返回值:  	无
//备注:     
//-----------------------------------------------
void api_TimeFormat645To698(BYTE *pTime645, TRealTimer *pTime698);

//-----------------------------------------------
//函数功能: 上电将info flash中的系数搬到rtc补偿寄存器中
//
//参数: 	无
//
//
//返回值:  	无
//
//备注:以后放在分钟里面，每分钟也要效验一下!!!!!!
//-----------------------------------------------
void api_Load_RtcInfoData( void );

//-----------------------------------------------
//函数功能: 软时钟
//
//参数: 	无
//
//
//返回值:  	无
//
//备注:目前根据F460的1ms中断进行计时，可通过SEL_DI_CONFIG进行选择
//-----------------------------------------------
void api_SoftClockSecTsk(void);

#endif//#ifndef __RTC_API_H

