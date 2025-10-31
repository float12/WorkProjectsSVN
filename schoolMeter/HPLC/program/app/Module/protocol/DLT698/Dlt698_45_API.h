//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	姜文浩 魏灵坤
//创建日期	2016.8.4
//描述		DL/T 698.45面向对象协议api头文件
//修改记录	
//----------------------------------------------------------------------
#ifndef __Dlt698_45_API_H
#define __Dlt698_45_API_H

#if(SEL_DLT698_2016_FUNC == YES  )
//-----------------------------------------------
//				宏定义
//-----------------------------------------------
#define MAX_RN_SIZE				    (24+1) //随机数最大长度       3*8+1=25
//卡相关宏定义需要外部引用
/***************************DAR定义*************************************/
#define	DAR_Success			 	0	//成功
#define	DAR_HardWare			1	//硬件失效  //仅用于规约解析正确，操作API函数不正确的情况下 不可随意使用
#define	DAR_TempFail			2	//暂时失效
#define	DAR_RefuseOp			3	//拒绝操作
#define	DAR_Undefined			4	//对象未定义
#define	DAR_ClassId				5	//对象接口类不符合
#define	DAR_NoObject			6	//对象不存在
#define	DAR_WrongType			7	//类型不匹配
#define	DAR_OverRegion			8	//越界
#define	DAR_DataBlock			9	//数据块不可用
#define	DAR_CancelDiv			10	//分帧传输已取消
#define	DAR_NotInDiv			11	//不处于分帧传输状态
#define	DAR_CancelBlockWr		12	//块写取消
#define	DAR_NoInBlockWr			13	//不存在块写状态
#define	DAR_BlockSeq			14	//数据块序号无效
#define	DAR_PassWord			15	//密码错/未授权
#define	DAR_ChangeBaud			16	//通信速率不能更改
#define	DAR_OverZone			17	//年时区数超
#define	DAR_OverSlot			18	//日时段数超
#define	DAR_OverRAte			19	//费率数超
#define	DAR_MatchAuth			20	//安全认证不匹配
#define	DAR_Recharge			21	//重复充值
#define	DAR_EsamFial			22	//ESAM验证失败
#define	DAR_AuthFail			23	//安全认证失败
#define	DAR_CustomNo			24	//客户编号不匹配
#define	DAR_ChargeTimes			25	//充值次数错误
#define	DAR_OverHold			26	//购电超囤积
#define	DAR_WrongAddr			27	//地址异常
#define	DAR_Symmetry			28	//对称解密错误
#define	DAR_Asymmetry			29	//非对称解密错误
#define	DAR_signature			30	//签名错误
#define	DAR_HangUp				31	//电能表挂起
#define	DAR_TimeStamp			32	//时间标签无效
#define	DAR_OverTime			33	//esam的P1P2不正确
#define	DAR_P1P2ERROW			34	//请求超时
#define	DAR_LCERROW				35	//请求超时
#define	DAR_OtherErr			255	//其它错误
//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
typedef enum
{
   	eData = 0,         	//只获取数据
   	eTagData,    		//数据+TAG
   	eAddTag,        	//只加TAG
   	eProtocolData,      //协议数据 
   	eMaxData = 0X55,    //最大源数据长度
}eDataTagChoice;//数据加TAG标志

typedef enum
{
   	eFREEZE_TIME_FLAG = 0x01, 		//时间标志
   	eRECORD_TIME_INTERVAL_FLAG = 0x02, //时间区间标志
   	eNUM_FLAG = 0x04,    				//次数标志
   	eEVENT_START_TIME_FLAG = 0x08,		//事件起始时间
   	eEVENT_END_TIME_FLAG = 0x10,		//事件结束时间
}eRecoredFlagChoice;

typedef struct TTimeIntervalPara_t	//读取记录参数结构体
{
    DWORD StartTime;				//起始时间
    DWORD EndTime;					//结束时间
    WORD  TI;						//时间间隔
    WORD  ReturnFreezeNum;			//读出冻结的条数（输出值）
    BYTE  AllCycle;                 //全周期标志 0X55代表全周期 0x00 非全周期
}TTimeIntervalPara;

typedef struct TDLT698RecordPara_t			//读取记录参数结构体
{
	WORD OI;								//类型标识OI
	BYTE Ch;                                //读取通道
	ePHASE_TYPE Phase;						//指定象限 0 代表总 1 代表A相  2 代表B相  3 代表 C相
	eRecoredFlagChoice eTimeOrLastFlag; 	//时间或次数的标志  0x00 时间  0x01 次数	
	DWORD TimeOrLast;						//读取事件或记录的时间
	TTimeIntervalPara TimeIntervalPara;     //时间段参数
	BYTE OADNum;							//要读取的数据项的OAD的个数 OAD个数为零 则返回全部
	BYTE *pOAD;								//要读取的数据项的OAD指针
}TDLT698RecordPara;

typedef enum
{
   	eConnectGeneral = 0, 	//一般建立应用链接 ---值不能随意变更-jwh
	eConnectMaster,  		//主站建立应用链接
	eConnectTerminal,		//终端建立应用链接
	eConnectModeMaxNum,		//最大个数
}eConnectMode;//数据加TAG标志

typedef enum
{
   	eGetNormalData = 0, 	//获取正常数据
   	eGetRecordData, 		//获取记录数据----注意电能返回脉冲数，仅冻结和事件使用，避免出错！！！！！-jwh
   							//返回电能的脉冲数长度为5个字节 存储冻结和读取冻结数据，判断冻结数据时使用，如果判断返回数据长度应用Normal
}eGetDataType;//数据加TAG标志

typedef struct TConnectInfo_t   	//链路层响应结构体
{
	DWORD ConnectValidTime;				//698建立应用连接时间(实时)
	DWORD ConstConnectValidTime;	 	//698建立应用固定连接时间
}TConnectInfot;

typedef struct TDLT698APPConnect_t   	//链路层响应结构体
{
	TConnectInfot ConnectInfo[eConnectModeMaxNum];
	DWORD IRAuthTime;			        //红外认证时间(实时)
	DWORD TerminalAuthTime;				//终端认证时间
}TDLT698APPConnect;
//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern TSafeModePara SafeModePara;
extern TDLT698APPConnect APPConnect;
extern BYTE g_EnableAutoReportFlag;//只为应付鼎信测试软件测试
extern __no_init BYTE g_PassWordErrCounter;//密码错误次数
extern BYTE g_byClrTimer_500ms;
//-----------------------------------------------
// 				函数声明
//-----------------------------------------------


//--------------------------------------------------
//功能描述:  698获取单个OAD数据的对外接口函数
//         
//参数:	
//		DataType[in]:	eData/eTagData/eAddTag
//		Dot[in]:  		获取数据的小数点位数，oxff--表示规约默认的小数位数（规约调用）
//		pOAD[in]：		获取数据的OAD
//		InBuf[in]：		给定要加Tag的数据(注:如果是事件类中的 事件上报状态(0x3300)、编程对象列表(0x3302)、事件清零列表(0x330c)时，第一个字节InBuf[0]需要存放Array的个数Number)
//		OutBufLen[in]：	给定OutBufLen的长度
//		OutBuf[out]:  	获取数据存放OutBuf中
//
//返回值: 返回数据长度	0x0000:数据长度不够  0x8000: 出现错误
//         
//备注内容:  无
//--------------------------------------------------
WORD api_GetProOadData( eGetDataType GetDataType, BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf );

//--------------------------------------------------
//功能描述: 获取单个OAD对应数据的长度
//         
//参数:    	GetDataType[in]	GetNormalData获取正常数据 GetRecordData 	 获取记录数据----注意电能返回脉冲数，仅冻结和事件使用，避免出错！！！！！-jwh
//			DataType[in]: 	eData/eTagData
//         	pOAD[in]：获取数据长度的OAD
//			Number[in]：Array的个数  
//
//返回值:   数据长度  0x0000:数据长度不够  0x8000: 出现错误
//         
//备注内容: Number:只有获取事件类长度时需要配置此参数，获取其他类长度时，此参数直接填写0
//--------------------------------------------------
WORD api_GetProOADLen( eGetDataType GetDataType, BYTE DataType, BYTE *pOAD, BYTE Number);

//--------------------------------------------------
//功能描述:  规约读取记录函数
//         
//参数:      BYTE Tag[in] 						//是否加TAG标志;	0x01代表需要加TAG； 	   	  0x00 代表不加TAG； 
//							       
//           TDLT698RecordPara *DLT698RecordPara[in] 	//读取记录或事件的参数结构体
//         
//           WORD InBufLen[in]						//输入的buf长度 用来判断buf长度是否够用
//         
//           BYTE *InBuf[in]						//输入buf指针
//         
//返回值:    WORD								// bit15位置1 代表记录或冻结无此类型 置0代表数据能正确返回； 
//											bit0~bit14 代表返回的冻结或事件的字节数
//         
//备注内容:  无
//--------------------------------------------------
WORD api_ReadProRecordData( BYTE Tag, TDLT698RecordPara *pDLT698RecordPara, WORD BufLen,BYTE *pBuf );

//-----------------------------------------------
//函数功能: 记录中的数据转化
//
//参数:
//		pOAD[in]: 输入OAD
//
//		Len[in]: 输入bufg长度
//
//		Buf[in]:指向电能的指针 返回hex
//
//		OutBufLen[in] 输出数据长度
//
//		OutBuf[in]	输出buf
//
//返回值:
//
//备注: 不能将数据反写到Buf中，因为申请的长度不够可能会超限，直接回写outbuf---jwh
//-----------------------------------------------
WORD api_RecordDataTransform( BYTE* pOAD, WORD* BufLen , BYTE* Buf, WORD OutBufLen,BYTE* OutBuf );

void api_ClrProSafeModePara(BYTE byType);//清零显式安全模式参数

//--------------------------------------------------
//功能描述:  判断时段表间隔是否大于15分钟
//         
//参数  :   BYTE *pBuf[in] 时段表BUF指针
//
//返回值:    WORD TRUE 有效  FALSE 无效 
//
//备注内容:  如果结算日读取失败，默认返回成功
//--------------------------------------------------
WORD api_JudgeTimeSegInterval(BYTE *pBuf);
//--------------------------------------------------
//功能描述:       判断广播校时是否发生在结算时刻前后5分钟
//         
//参数  :       无
//
//返回值:        TRUE--超过5min    
//			  FALSE--5min以内
//备注内容: 
//--------------------------------------------------
BOOL JudgeBroadcastMeterTime( WORD Sec );
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
WORD api_GetMeterStatus( ePROTOCO_TYPE ProtocolType, BYTE Type, WORD BufLen, BYTE *Buf );
//--------------------------------------------------
//功能描述:断开应用链接
//         
//参数  :   无
//
//返回值:    无   
//         
//备注内容:  无
//--------------------------------------------------
void api_Release698_Connect( void );
//--------------------------------------------------
//功能描述:   	处理主动上报事件列表
//         
//参数: 
//			OutBufLen[out]：	buf长度    
//         	OutBuf[out]：
//返回值:  	返回数据长度
//         
//备注内容: 不处理 class7（属性2），class24（属性6~9）
//--------------------------------------------------
WORD AddActiveReportList(WORD OutBufLen, BYTE *OutBuf );
//--------------------------------------------------
//功能描述:  链路层数据响应函数
//         
//参数:     pAPDU[IN]	 //apdu 指针
//		  APDU_Len[IN]//apdu 长度 无需进行极限值判断
//         
//返回值:    
//         
//备注内容:  无
//--------------------------------------------------
void api_ActiveReportResponse(BYTE* pAPDU, WORD APDU_Len ,BYTE Type);

//--------------------------------------------------
//功能描述: 把时段参数多余的时区、时段用最后一个有效的补上
//         
//参数:    BYTE byLen[in]            设置数据长度
//         
//         BYTE byMaxLen[in]         最大数据长度
//
//         BYTE *pData[in]           数据指针
//         
//返回值:     BYTE DAR结果 
//         
//备注内容:    参数在全局变量 ProtocolBuf 中
//--------------------------------------------------
void api_AddTimeSegParaExtDataWithLastValid(BYTE byLen, BYTE byMaxLen, BYTE *pBuf );
//--------------------------------------------------
// 功能描述:  读分钟冻结数据时，由于存电器个数不确定，需要读取时进行计算
//
// 参数:      [in]：Num
//
// 返回值:    无
//
// 备注内容:  无
//--------------------------------------------------
void api_SetTagArrayNum(BYTE Num);
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
WORD api_DealMoneyDataToESAM( DWORD Money, DWORD BuyTimes, BYTE *pCustomCode, BYTE *pSIDMAC );
#endif//#if( PREPAY_MODE == PREPAY_LOCAL)

#endif//(SEL_DLT698_2016_FUNC == YES )
#endif//__DLT698_45_2016_API_H