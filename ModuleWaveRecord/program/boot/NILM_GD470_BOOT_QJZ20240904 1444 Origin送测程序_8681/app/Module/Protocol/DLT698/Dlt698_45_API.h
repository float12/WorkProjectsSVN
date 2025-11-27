//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	姜文浩 魏灵坤
//创建日期	2016.8.4
//描述		DL/T 698.45面向对象协议api头文件
//修改记录	
//----------------------------------------------------------------------
#ifndef __Dlt698_45_API_H
#define __Dlt698_45_API_H
//-----------------------------------------------
//				宏定义
//-----------------------------------------------
#define MAX_RN_SIZE				    (24+1) //随机数最大长度       3*8+1=25
//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
typedef enum
{
   	eData = 0,         	//只获取数据
   	eTagData,    		//数据+TAG
   	eAddTag,        	//只加TAG
   	eProtocolData,      //协议数据 
   	eMaxData = 0X55    //最大源数据长度
}eDataTagChoice;//数据加TAG标志

typedef enum
{
   	eFREEZE_TIME_FLAG = 0x01, 		//时间标志
   	eRECORD_TIME_INTERVAL_FLAG = 0x02, //时间区间标志
   	eNUM_FLAG = 0x04,    				//次数标志
   	eEVENT_START_TIME_FLAG = 0x08,		//事件起始时间
   	eEVENT_END_TIME_FLAG = 0x10		//事件结束时间
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

typedef struct TDLT698APPConnect_t   	//链路层响应结构体
{
	BYTE Mode;						    //建立应用链接模式 0X00:一般密码应用链接               0x55：对称密码应用链接
	DWORD ConnectValidTime;				//698建立应用连接时间(实时)
	DWORD ConstConnectValidTime;	 	//698建立应用固定连接时间
	BYTE  ConnectChannel;				//698建立应用的通道
	DWORD IRAuthTime;			        //红外认证时间(实时)
}TDLT698APPConnect;

typedef struct TCoefficient_t
{
	DWORD UK;
	DWORD IK;
	DWORD Phase;
}TCoefficient;//周波系数

typedef enum
{	
	eGET_ADDR = 0,
	eGET_SAMPLENUM,
	eGET_SAMPLEINFO,
	eGET_RTC,
	eMAXCOMNO
}eCOMStatus;//与表格一一对应，单调递增

typedef struct TManageComInfo_t   	//加TAG时Buf使用的参数
{
	eCOMStatus Status;
	BYTE AddressType;
	BYTE SampleNO;//1： 单相表   	2：三相三		3:三相四
	WORD DelayTime;
	BYTE Address[6];
	TCoefficient Coefficient;
	WORD EventReportOverTime;
}TManageComInfo;

//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern TDLT698APPConnect APPConnect;
extern BYTE g_EnableAutoReportFlag;//只为应付鼎信测试软件测试
extern BYTE g_byClrTimer_500ms;
extern TManageComInfo ManageComInfo;
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
extern WORD api_GetProOadData( BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf );

//--------------------------------------------------
//功能描述: 获取单个OAD对应数据的长度
//         
//参数:    	DataType[in]: 	eData/eTagData
//         	pOAD[in]：获取数据长度的OAD
//			Number[in]：Array的个数  
//
//返回值:   数据长度  0x0000:数据长度不够  0x8000: 出现错误
//         
//备注内容: Number:只有获取事件类长度时需要配置此参数，获取其他类长度时，此参数直接填写0
//--------------------------------------------------
extern WORD api_GetProOADLen(BYTE DataType, BYTE *pOAD, BYTE Number);

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
extern WORD api_ReadProRecordData( BYTE Tag, TDLT698RecordPara *pDLT698RecordPara, WORD BufLen,BYTE *pBuf );

extern void api_ClrProSafeModePara(BYTE byType);//清零显式安全模式参数


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
extern void api_AddTimeSegParaExtDataWithLastValid(BYTE byLen, BYTE byMaxLen, BYTE *pBuf );
#endif//(SEL_DLT698_2016_FUNC == YES )
