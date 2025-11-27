//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部
//创建人	姜文浩 魏灵坤
//创建日期	2016.8.4
//描述		DL/T 698.45面向对象协议C文件
//修改记录
//----------------------------------------------------------------------
#include "AllHead.h"
#include "Dlt698_45.h"

//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
// 索引信息在EEPROM中的地址范围


//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------


//-----------------------------------------------
//				全局使用的变量，常量
BYTE RN[MAX_RN_SIZE];
BYTE g_TimeTagBuf[12];
BYTE g_byClrTimer_500ms;
BYTE ClientAddress;//客户机地址
BYTE ConnectMode;  //连接模式

TDLT698LinkData LinkData[MAX_COM_CHANNEL_NUM];
TDLT698APPData APPData[MAX_COM_CHANNEL_NUM];
BYTE DLT698APDURequestBuf[MAX_APDU_SIZE+EXTRA_BUF];
BYTE ProtocolBuf[MAX_APDU_SIZE+EXTRA_BUF];// MAX_APDU_SIZE = 2000, EXTRA_BUF = 30

TDLT698APPConnect APPConnect;
TSafeModePara SafeModePara;
TAPDUBufFlag APDUBufFlag;
TConsultData ConsultData;
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
																		
const DWORD EventPhaseUnit[8][2] =			
{ 	//分相事件记录单元
	{0X00000004,0x00000000 },//行数
	{0x00022220,0x00022220 },//事件记录序号
	{0x00021e20,0x00021e20 },//事件发生时间
	{0x00022020,0x00022020 },//事件结束时间
	{0x00020033,0x00020033 },//事件上报状态
} ;		

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
static void ProcDlt698App( BYTE Ch);
static void DealGet_Request(BYTE Ch,BYTE *pAPDU);
static WORD AnalyseDlt698ApduLen( BYTE Ch);
eAPPBufResultChoice  DLT698AddBuf(BYTE Ch, BYTE Mode,BYTE *Buf,WORD Len);
eAPPBufResultChoice GetRequestNormal( BYTE Ch,BYTE *pOAD );
eAPPBufResultChoice GetRequestNormalList( BYTE Ch);
eAPPBufResultChoice GetRequestRecord( BYTE Ch,BYTE *pOAD );
eAPPBufResultChoice GetRequestRecordList( BYTE Ch );
//-----------------------------------------------
//				函数定义
//-----------------------------------------------


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
	for( i=0; i<MAX_COM_CHANNEL_NUM; i++ )
	{
		Serial[i].Addr_Len = 6;//通信地址默认6字节
	}
	PowerUpInitAPDUBufFlag();
	//lib_CheckSafeMemVerify( (BYTE *)&SafeModePara, sizeof(SafeModePara), REPAIR_CRC );//TSafeModePara RAM中校验暂时没用上
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
	eResultChoice = eAPP_ERR_RESPONSE;
	
	switch(pAPDU[1])
	{		
	case GET_REQUEST_NORMAL:
		eResultChoice = GetRequestNormal( Ch,&pAPDU[3] );
		break;
	case GET_REQUEST_NORMALLIST:	
		eResultChoice = GetRequestNormalList(Ch);
		break;
	case GET_REQUEST_RECORD:
		eResultChoice = GetRequestRecord(Ch, &pAPDU[3] );
		break;
	case GET_REQUEST_RECORDLIST:	 
		eResultChoice = GetRequestRecordList(Ch);
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
			Proc698LinkDataRequest(i);		//链路层请求处理函数
			ProcDlt698App(i);             	//应用层处理函数
			Proc698LinkDataResponse(i);		//链路层响应处理函数
			break;
		}
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
	return DEFAULT_MAX_PRO_BUF_485;
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
   
   return ConsultLen;
}

//--------------------------------------------------
//功能描述:  判断时间标签函数
//         
//参数:     BYTE Ch[in]
//         
//返回值:    WORD  TRUE 有效  FALSE 无效  
//         
//备注内容:  无
//--------------------------------------------------
static WORD JudgeTimeTagValid(BYTE Ch)
{
//	DWORD dwTime;//unsigned long int
//	BYTE TIIndex;
//	WORD wTI;
	WORD Len;
//	BYTE *pBuf;
//  TRealTimer TimeTag;
	
	if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return FALSE;
    }

	Len = AnalyseDlt698ApduLen( Ch );
	if( Len == 0x8000 )
	{
       return FALSE;
	}
	
//	pBuf = (BYTE *)&(LinkData[Ch].pAPP[Len]);
	LinkData[Ch].pAPPLen.w -= 1;//剔除OPTIONAL 1个字节

	APPData[Ch].TimeTagFlag = eNO_TimeTag;//默认无时间标签
/*
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
        
        TIIndex = pBuf[1+7];
        if( TIIndex > 5 )
        {
            APPData[Ch].TimeTagFlag = eTime_Invalid;//时间标签无效
            return TRUE;
        }
        
        lib_ExchangeData((BYTE*)&wTI, (BYTE*)&pBuf[1+7+1], 2);
        
        dwTime = api_CalcInTimeRelativeMin(&TimeTag);//CalcTime 内调用 api_CheckClock
        if( dwTime == ILLEGAL_VALUE_8F )//CalcTime 内调用 api_CheckClock 没通过
        {
            APPData[Ch].TimeTagFlag = eTime_Invalid;//时间无效;
            return TRUE;
        }   
        if( dwTime >= g_RelativeMin )
        {
            APPData[Ch].TimeTagFlag = eTime_True;//有时间标签;
            return TRUE;
        }
        
        dwTime = TimeIntervalToAbsMin( dwTime, TIIndex, wTI );
        
        if( dwTime > g_RelativeMin )
        {
            APPData[Ch].TimeTagFlag = eTime_True;//有时间标签;
            return TRUE;
        }
	}
	*/
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
	//接口类24
    if((OI&0XF000) == 0x3000)
    {
        if(     ( ClassAttribute != 6 )
            &&  ( ClassAttribute != 7 )
            &&  ( ClassAttribute != 8 )
            &&  ( ClassAttribute != 9 ) )
        {
            return FALSE;
        }
    }
    else if((OI&0XF000) == 0x5000)
    {
        if( ClassAttribute != 2 )
        {
            return FALSE;
        }
    }
    else
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
    return 0;
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
eAPPBufResultChoice  DLT698AddBuf(BYTE Ch, BYTE Mode,BYTE *Buf,WORD Len)
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

//--------------------------------------------------
//功能描述:  数组元素索引不为零的情况下对数据进行处理
//         
//参数:      BYTE   *pData[in] 需要判断的数据指针
//         
//返回值:     BYTE*  返回处理后的数据指针
//         
//备注内容:    无
//--------------------------------------------------
//static BYTE *DealArrayClassIndexNotZeroData( BYTE *pData )
//{
//	pData = pData -2;//将数据指针偏移两个指针
//	pData[0] = Array_698;
//	pData[1] = 1;
//	return pData;
//}
//--------------------------------------------------
//功能描述:  结构体元素索引不为零的情况下对数据进行处理
//         
//参数:     BYTE * pData[in]
//         
//返回值:    BYTE*  返回处理后的数据指针
//         
//备注内容:  无
//--------------------------------------------------
//static BYTE *DealStructureClassIndexNotZeroData( BYTE *pData )
//{
//	pData = pData -2;//将数据指针偏移两个指针
//	pData[0] = Structure_698;
//	pData[1] = 1;
//	return pData;
//}

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
eAPPBufResultChoice GetRequestNormal( BYTE Ch,BYTE *pOAD )
{
	WORD Len;
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
	
    APPData[Ch].BufDealLen.w +=4;//已处理4字节oad

    if( APPData[Ch].TimeTagFlag == eTime_Invalid )
    {
		eResultChoice = DLT698AddBufDAR( Ch,DAR_TimeStamp );
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
            Len = GetProOadData( Ch, 1,0XFF,pOAD, (BYTE *)NULL,(APPData[Ch].APPMaxBufLen.w- APPData[Ch].APPCurrentBufLen.w-1) , &APPData[Ch].pAPPBuf[APPData[Ch].APPCurrentBufLen.w+1]);//添加数据buf
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
		eResultChoice = GetRequestNormal( Ch,&LinkData[Ch].pAPP[APPData[Ch].BufDealLen.w] );

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
	
	if((pDLT698RecordPara->OI >= 0x3000) && (pDLT698RecordPara->OI <= 0x310f))//调用事件函数
	{
      	// Len = api_ReadEventRecord( Tag, pDLT698RecordPara, BufLen, pBuf);
          Len = 0;
	}
	else if((pDLT698RecordPara->OI >= 0x5000) && (pDLT698RecordPara->OI <= 0x5011))//调用冻结函数
	{
		// Len = api_ReadFreezeRecord( Tag, pDLT698RecordPara, BufLen, pBuf);
        Len = 0;
	}

	return Len;//返回数据对象不存在
}

//---------------------------------------------------------------
//函数功能: 获得事件单元
//
//参数: OI[in]  事件OI
//
//返回值:  事件单元地址
//
//备注:
//---------------------------------------------------------------
static const DWORD (*GetEventUnit(WORD OI))[2]
{
    const DWORD(*pEventUnit)[2];

    switch( OI )
    {
        case OAD_HARMONIC_U_DISTORTION:
		case OAD_HARMONIC_I_DISTORTION:	
		case OAD_SHORT_FLICKER_U_OVERRUN:
		case OAD_LONG_FLICKER_U_OVERRUN:	
		case OAD_SAG_U:
		case OAD_SWELL_U:
		case OAD_INTERRUPTION_U:
        {
            pEventUnit = EventPhaseUnit;
            break;
        }
		case OAD_HARMONIC_U_OVERRUN:
		case OAD_HARMONIC_I_OVERRUN:
		case OAD_HARMONIC_P_OVERRUN:
		case OAD_INTER_HARMONIC_U_OVERRUN:
		case OAD_INTER_HARMONIC_I_OVERRUN:
		case OAD_INTER_HARMONIC_P_OVERRUN:
        {
            pEventUnit = StandardEventUnit;
            break;
        }
        default:
            pEventUnit = EventPhaseUnit;
            break;
    }

    return pEventUnit;
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
		if( ((pDLT698RecordPara->OI) >= 0x3000) && ((pDLT698RecordPara->OI) <= 0x3050))//事件RCSD
		{
		    //获取对应的事件记录表
		    pEvent = GetEventUnit( pDLT698RecordPara->OI );

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
	BYTE Num;
	BYTE *pRCSDNum;
	eAPPBufResultChoice eResultChoice;
	const DWORD (*pEvent)[2];
    WORD i,Len;

	pRCSDNum = &APPData[Ch].pAPPBuf[APPData[Ch].APPCurrentBufLen.w];//取得rcsd个数的指针
	eResultChoice = DLT698AddOneBuf( Ch, 0, 1);				//添加rcsd的 个数
	
	if( (OI >= 0x3000) && ( OI < 0x3050))	//事件RCSD
	{
	    //获取对应的事件记录表
	    pEvent = GetEventUnit( OI );

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
	
		// Len = api_ReadEventAttribute(OI,0,MAX_APDU_SIZE, ProtocolBuf);//返回结果无法处理 因此不判断
        Len = 0;

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
		
		// Len = api_ReadFreezeAttribute(OI,0,MAX_APDU_SIZE, ProtocolBuf);//返回结果无法处理 因此不判断
        Len = 0;

		if( (Len & 0x8000) ||(Len < 8))//返回错误或者长度小于1个oad长度
		{
			Num =0;
		}
		else
		{
			Num = Len/8;
		}
		
		if( Num > 20 )//进行极限值的判断
		{
			Num = 20;
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
	    if( Len.w > (MAX_ATTRIBUTE+10) )//下发的oad个数不能超过Buf长度
	    {
            DAR[0] = DAR_WrongType;
            return eResultChoice;
	    }
	    
		eResultChoice = DLT698AddBuf(Ch,0, pRCSD, (Len.w*5+LenOffest));//添加RCSD数据 不是完整数据
		if( eResultChoice != eADD_OK )
		{
			return eResultChoice;
		}
		
		if( (OI >= 0x3000) && ( OI < 0x3050))	//事件RCSD
		{
		    //获取对应的事件记录表
            pEvent = GetEventUnit( OI );

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
			if( (OI == 0x3007) || (OI == 0x300B) )//OI不是潮流反向和无功超限 分项事件只有A,B,C相
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
//    else if( JudgeTaskAuthority( Ch, eREAD_MODE, pRSD-4) == FALSE )//判断安全模式参数 优先级别高
//    {			
//        DAR = DAR_PassWord;
//    }
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
    return eAPP_ERR_RESPONSE;
/*	BYTE *pRCSD,DAR,LenOffest;
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

	return eResultChoice;*/
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
	BYTE Errow,LenOffest;
	BYTE Buf[(MAX_ATTRIBUTE+10)*4+20];
	WORD Len;
	WORD No,Result;
	TTwoByteUnion TmpLen;
	BYTE *pRCSD;
	eAPPBufResultChoice eResultChoice;
    TDLT698RecordPara DLT698RecordPara;
	
	No =1;
	Errow = 0x00;//错误标志 0：无错误  0x55：有错误未添加错误信息 0x77：有错误且已添加错误信息 
	pRCSD = pRSD+2;
	No = pRSD[1];//获取读取上几次
		
	//RCSD
	LenOffest =  Deal_698DataLenth( pRCSD, TmpLen.b, ePROTOCOL_TYPE ); 
	eResultChoice = DealRCSDData( Ch, OI, pRCSD,Buf, (BYTE *)&Errow);
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	
    if( APPData[Ch].TimeTagFlag == eTime_Invalid )//时间标签错误
    {
        eResultChoice = DLT698AddBufDAR( Ch,DAR_TimeStamp );
        if( eResultChoice != eADD_OK )
        {
        	return eResultChoice;
        }
        
        Errow = 0x77;
    }
//	else if( JudgeTaskAuthority( Ch, eREAD_MODE, pRSD-4) == FALSE )//判断安全模式参数
//	{			
//		eResultChoice = DLT698AddBufDAR( Ch,DAR_PassWord );
//		if( eResultChoice != eADD_OK )
//		{
//			return eResultChoice;
//		}
//		
//		Errow = 0x77;
//	}
	else if( JudgeRecordOAD(OI,ClassAttribute) == FALSE )//判断记录OAD
	{			
		eResultChoice = DLT698AddBufDAR( Ch,DAR_WrongType );
		if( eResultChoice != eADD_OK )
		{
			return eResultChoice;
		}
		Errow = 0x77;
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
            Errow = 0x55;
        }
	}
	
	if( Errow == 0x00 )//无错误 读取记录
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
	    if( Errow == 0x55 )
	    {
            eResultChoice = DLT698AddBufDAR( Ch, DAR_WrongType );
            if( eResultChoice != eADD_OK )
            {
                return eResultChoice;
            }
	    }
	}
	
	APPData[Ch].BufDealLen.w += 4+2+LenOffest+TmpLen.w*5;// 添加已处理下行buf长度 4 oad + 2 方法9 + 1个数 +rcsd

	return eResultChoice;
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
	
	eResultChoice = eAPP_ERR_RESPONSE;
	lib_ExchangeData(OI.b,pData,2);//获取冻结OI
	ClassAttribute = pData[2];
	pRSD = pData + 4;//获取RSD指针

	switch(pRSD[0])//选择方法
	{
		case 0:
			break;
			
		case 1://选择方法1
		
			eResultChoice = GetRequestRecordDataMethod1( Ch, pRSD, OI.w,ClassAttribute);
			
			break;
		case 2://选择方法2 为指定对象区间内连续间隔值 
	
			//eResultChoice = GetRequestRecordDataMethod2( Ch, pRSD, OI.w,ClassAttribute);

			break;
	
		case 9://选择方法9 读取上几次
		
			eResultChoice = GetRequestRecordDataMethod9( Ch, pRSD, OI.w,ClassAttribute);
			
			break;
			
		default:
			return  eAPP_ERR_RESPONSE;
	}
	
	return eResultChoice;
}

//--------------------------------------------------
//功能描述:	处理获取Normal响应的数据
//         
//参数:		BYTE Ch[in]     		通道选择
//         	BYTE *pOAD[in]  		OAD数据指针
//         
//返回值:	eAPPBufResultChoice     
//         
//备注内容:	无
//--------------------------------------------------
eAPPBufResultChoice GetResponseNormal( BYTE Ch, BYTE *pOAD )
{
 	BYTE ClassAttribute,i,j,Num;
	WORD Offset;
	DWORD dwDataU[3], dwDataI[4];
	// DWORD RelativeTime;
	float FloatData[5],dwDataPhase[3],Point;
    TTwoByteUnion OI;
	TRealTimer TmpRealTimer;//时间结构体

	APPData[Ch].APPFlag = APP_NO_DATA;
	lib_ExchangeData( OI.b, pOAD, 2 );
	ClassAttribute = pOAD[2];
	
	switch( OI.w )
	{
		case 0x4000://日期时间
			if( ClassAttribute == 2 )//属性2
			{
				if(pOAD[5] == DateTime_S_698)
				{
					lib_InverseData(&pOAD[6], 2);
					memcpy((BYTE*)&(TmpRealTimer.wYear),&pOAD[6],sizeof( TRealTimer ));
					api_WriteMeterTime(&TmpRealTimer);
					// RelativeTime = api_GetRelativeMin();
					// api_UpdateFreezeTime(RelativeTime);
					api_DealManageCOMStatus( eGET_RTC );
					// if(InitComplete == 0)
			        // {
			        //     InitComplete = 1;   //上电交互完成，置位初始化完成，绿灯灭
			        // }
				}
			}
		break;

		case 0x4001://通信地址
		  	if( ClassAttribute == 2 )//属性2
			{
			  	if( (pOAD[5] == Octet_string_698) && (pOAD[6] == 6) )
				{
					memcpy( ManageComInfo.Address, &pOAD[7], 6 );//获取通信地址
					ManageComInfo.AddressType = 0x55;
					api_DealManageCOMStatus( eGET_ADDR );
				}
			}
			break;
			
		case 0x4010://计量元件数
			if( ClassAttribute == 2 )
			{
				if( pOAD[5] == Unsigned_698 )
				{
					ManageComInfo.SampleNO = pOAD[6];
					api_DealManageCOMStatus( eGET_SAMPLENUM );
				}
			}
			break;
					
		case 0x4804://计量配置参数
			if( ClassAttribute == 2 ) //属性2，只读
			{
			    Offset = 5;
			    if( pOAD[Offset++] != Structure_698 )   return eAPP_ERR_RESPONSE;
			    if( pOAD[Offset++] != 0x05 )    return eAPP_ERR_RESPONSE;

			    //区间计量系数
			    if( pOAD[Offset++] != Array_698 )    return eAPP_ERR_RESPONSE;
			    Num = pOAD[Offset];
			    for( i=0; i<Num; i++)
			    {
			        if(i == 0) Offset++;
			        if( pOAD[Offset++] != Structure_698 )   return eAPP_ERR_RESPONSE;
			        if( pOAD[Offset++] != 0x04 )   return eAPP_ERR_RESPONSE;
			        //Region
			        Offset += 12;
			        //电压系数
			        if( pOAD[Offset++] != Array_698 )    return eAPP_ERR_RESPONSE;
			        if( pOAD[Offset++] != MAX_PHASE )    return eAPP_ERR_RESPONSE;
			        for( j=0; j<MAX_PHASE; j++ )
			        {
			            if( pOAD[Offset++] != Double_long_unsigned_698 )    return eAPP_ERR_RESPONSE;
			            lib_ExchangeData( (BYTE*)&dwDataU[j], pOAD+Offset, 4 );
			            Offset += 4;
			        }
			        //电流
                    if( pOAD[Offset++] != Array_698 )    return eAPP_ERR_RESPONSE;
                    if( pOAD[Offset++] != MAX_PHASE )    return eAPP_ERR_RESPONSE;
                    for( j=0; j<MAX_PHASE; j++ )
                    {
                        if( pOAD[Offset++] != Double_long_unsigned_698 )    return eAPP_ERR_RESPONSE;
                        lib_ExchangeData( (BYTE*)&dwDataI[j], pOAD+Offset, 4 );
                        Offset += 4;
                    }
                    //相角
                    if( pOAD[Offset++] != Array_698 )    return eAPP_ERR_RESPONSE;
                    if( pOAD[Offset++] != MAX_PHASE )    return eAPP_ERR_RESPONSE;
                    for( j=0; j<MAX_PHASE; j++ )
                    {
                        if( pOAD[Offset++] != Double_long_unsigned_698 )    return eAPP_ERR_RESPONSE;
                        lib_ExchangeData( (BYTE*)&dwDataPhase[j], pOAD+Offset, 4 );
                        Offset += 4;
                    }
			    }

			    //区间零线电流系数
			    if( pOAD[Offset++] != Array_698 )    return eAPP_ERR_RESPONSE;
			    Num = pOAD[Offset];
                for( i=0; i<Num; i++)
                {
                    if(i == 0) Offset++;
                    if( pOAD[Offset++] != Structure_698 )   return eAPP_ERR_RESPONSE;
                    if( pOAD[Offset++] != 0x02 )   return eAPP_ERR_RESPONSE;
                    //Region
                    Offset += 12;
                    //零线电流系数
                    if( pOAD[Offset++] != Double_long_unsigned_698 )    return eAPP_ERR_RESPONSE;
                    lib_ExchangeData( (BYTE*)&dwDataI[3], pOAD+Offset, 4 );
                    Offset += 4;
                }

                //小数位数
                if( pOAD[Offset++] != Unsigned_698 )   return eAPP_ERR_RESPONSE;
                Point = pOAD[Offset++];

                //采样周波点数
                if( pOAD[Offset++] != Long_unsigned_698 )   return eAPP_ERR_RESPONSE;
                Offset += 2;

                //采样数据长度
                if( pOAD[Offset++] != Unsigned_698 )   return eAPP_ERR_RESPONSE;

                for( i=0; i<MAX_PHASE; i++ )
                {
                    //电压系数
                    FloatData[0] = (FLOAT)dwDataU[i]/pow(10.0,Point);
                    Set_Adjust_Para( Factor_Big_Vol, i, &FloatData[0] );
                    //电流系数
                    FloatData[1] = (FLOAT)dwDataI[i]/pow(10.0,Point);
                    Set_Adjust_Para( Factor_Big_Cur, i, &FloatData[1] );
                    //相角系数
                    FloatData[2] = dwDataPhase[i];
                    Set_Adjust_Para( Factor_Big_Ang, i, &FloatData[2] );
                    //功率系数
                    FloatData[3] = FloatData[0] * FloatData[1];
                    Set_Adjust_Para( Factor_P, i, &FloatData[3] );
                    FloatData[4] = FloatData[3] * Q_Ratio;
                    Set_Adjust_Para( Factor_Q, i, &FloatData[4] );
                }

                api_DealManageCOMStatus( eGET_SAMPLEINFO );
			}
			break;	
			
		default:
			break;
	}

	return eADD_OK;
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
BYTE SetRTC(BYTE ClassAttribute, BYTE ClassIndex, BYTE *pData) //暂未考虑ClassIndex不为零的情况
{
	BYTE DAR;
	WORD Result;
	// DWORD RelativeTime;
	TRealTimer TmpRealTimer; //时间结构体
	TFourByteUnion OAD;

	DAR = DAR_WrongType;
	memcpy(OAD.b, pData - 4, 4); //获取OAD

	if ((ClassAttribute != 0x02) || (ClassIndex != 0)) //只能设置属性2
	{
		return DAR_Undefined;
	}

	if (pData[0] != DateTime_S_698)
	{
		return DAR_Undefined;
	}

	// RelativeTime = api_GetRelativeMin();
	lib_InverseData(&pData[1], 2);
	memcpy((BYTE *)&(TmpRealTimer.wYear), &pData[1], sizeof(TRealTimer));
	Result = api_WriteMeterTime(&TmpRealTimer);
	if (Result == FALSE)
	{
		DAR = DAR_HardWare; //此为固定错误返回，不可随意更改，与上层调用函数有关
	}
	else
	{
		api_SetAllTaskFlag(eFLAG_TIME_CHANGE);
		// api_UpdateFreezeTime(RelativeTime);
		DAR = DAR_Success; //DAR 成功
	}
	return DAR;
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
eAPPBufResultChoice SetRequestParameter(BYTE Ch, BYTE *pOAD)
{
	BYTE DAR, ClassAttribute, ClassIndex;
	BYTE *pData;
	TTwoByteUnion OI;
	eAPPBufResultChoice eResultChoice;

	pData = &pOAD[4];				 //获取数据指针
	lib_ExchangeData(OI.b, pOAD, 2); //获取OI
	ClassAttribute = (pOAD[2] & 0x0f);
	ClassIndex = pOAD[3];
	DAR = DAR_WrongType;

	switch (OI.w)
	{
	case 0x4000: //日期时间
		DAR = SetRTC(ClassAttribute, ClassIndex, pData);
		break;
	default:
		DAR = DAR_WrongType;
		break;
	}

	eResultChoice = DLT698AddOneBuf(Ch, 0x55, DAR); //是完整数据

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
eAPPBufResultChoice SetRequestNormal(BYTE Ch, BYTE *pOAD)
{
	TTwoByteUnion OI;
	eAPPBufResultChoice eResultChoice;
	BYTE OAD[4];
	WORD DataLen;

	if (Ch >= MAX_COM_CHANNEL_NUM) //进行通道判断
	{
		return eAPP_ERR_RESPONSE;
	}

	lib_ExchangeData(OI.b, pOAD, 2);
	eResultChoice = DLT698AddBuf(Ch, 0, pOAD, 4); //添加OAD数据 不是完整数据
	if (eResultChoice != eADD_OK)
	{
		return eResultChoice;
	}

	memcpy(OAD, pOAD, 4); //保存编程OAD;

	DataLen = GetProtocolDataLen(pOAD + 4); //获取Data长度 DataLen的错误长度在后面进行了判断

	if ((OI.w >= 0x4000) && (OI.w <= 0x4517)) //设置参变量
	{
		eResultChoice = SetRequestParameter(Ch, pOAD);
	}
	else //其他
	{
		eResultChoice = DLT698AddOneBuf(Ch, 0x55, DAR_Undefined); //是完整数据
	}
	if (DataLen == 0x8000) //长度不能正常解析，不再进行list数据处理
	{
		return eAPP_RETURN_DATA;
	}
	APPData[Ch].BufDealLen.w += (DataLen + 4);
	return eResultChoice;
}
//--------------------------------------------------
//函数功能：   698设置响应
//
//参数:      BYTE Ch[in]      通道选择
//
//         BYTE *pAPDU[in]  APDU指针
//返回值：  无
//
//备注：
//-------------------------------------------------
void DealSet_Request(BYTE Ch, BYTE *pAPDU)
{
	eAPPBufResultChoice eResultChoice;

	if (Ch >= MAX_COM_CHANNEL_NUM) //进行通道判断
	{
		return;
	}

	DLT698AddBuf(Ch, 0, LinkData[Ch].pAPP, 3); //添加命令头
	APPData[Ch].BufDealLen.w = 3;
	eResultChoice = eAPP_ERR_RESPONSE;

	switch (pAPDU[1])
	{
	case SET_REQUEST_NORMAL:
		eResultChoice = SetRequestNormal(Ch, (BYTE *)&(pAPDU[3]));
		break;
	default:
		break;
	}

	if (eResultChoice == eAPP_NO_RESPONSE)
	{
		APPData[Ch].APPFlag = APP_NO_DATA;
	}
	else if (eResultChoice == eAPP_ERR_RESPONSE)
	{
		APPData[Ch].APPFlag = APP_ERR_DATA;
	}
	else
	{
		APPData[Ch].APPFlag = APP_HAVE_DATA;
	}
}

//--------------------------------------------------
//功能描述:    698读取响应
//         
//参数:      BYTE Ch[in]      通道选择
//         
//         BYTE *pAPDU[in]  APDU指针
//         
//返回值:     无  
//         
//备注内容:    无
//--------------------------------------------------
void DealGet_Response(BYTE Ch,BYTE *pAPDU )
{	
  	BYTE i;
    TTwoByteUnion Len;
	WORD DataLen;
  	
    if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return;
    }
	
	
	switch(pAPDU[1])
	{		
	case GET_REQUEST_NORMAL:
		GetResponseNormal( Ch,&pAPDU[3] );
		break;
	case GET_REQUEST_NORMALLIST:	
		DataLen = 0;
	  	for( i=0; i<pAPDU[3]; i++ )
		{		
			GetResponseNormal( Ch, &pAPDU[4+4*i+DataLen] );
					
			if( pAPDU[4+4*i+DataLen+4] == 1 )//后验获取数据长度
			{
				Len.w = GetProtocolDataLen( &pAPDU[4+4*i+DataLen+5] );
				DataLen += (1 + Len.w);//Len(Result) + Len(数据长度)
			}
			else
			{
				DataLen += 2;
			}
		}
		break;
	case GET_REQUEST_RECORD:
		break;
	case GET_REQUEST_RECORDLIST:	 
		break;
	default:
		break;
	}
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
eAPPBufResultChoice GetRequestRecord( BYTE Ch,BYTE *pOAD )
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

    DAR = DAR_WrongType;
    lib_ExchangeData(OI.b,pOMD,2);//取oi
    pData = &pOMD[4];//取data
    DAR = DAR_WrongType;//默认失败

    if( pData[0] == NULL_698 )//数据为NULL
    {
        switch( pOMD[2] )
        {
            case 3://数据初始化

                DAR = DAR_Success;
            break;

            default:
                 DAR = DAR_WrongType;//默认失败
            break;
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
//-----------------------------------------------
//功能描述: 操作函数
//参数:
//         BYTE Ch[in]       通道选择
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
    BYTE OMD[4];

    if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return eAPP_ERR_RESPONSE;
    }

    lib_ExchangeData(OI.b,pOMD,2);
    memcpy( OMD, pOMD, 4 );

    eResultChoice = DLT698AddBuf(Ch,0, pOMD, 4);//添加OAD数据 不是完整数据
    if( eResultChoice != eADD_OK )
    {
        return eResultChoice;
    }

    DataLen = GetProtocolDataLen( pOMD+4);//获取Data长度 DataLen的错误长度在后面进行了判断

    if( APPData[Ch].TimeTagFlag == eTime_Invalid )//时间标签错误
    {
        eResultChoice = DLT698AddOneBuf( Ch, 0x55, DAR_TimeStamp);//DAR
        if( eResultChoice != eADD_OK )
        {
            return eResultChoice;
        }
        eResultChoice = DLT698AddOneBuf( Ch, 0x55, 0);//DATA OPTINAL
    }
    else if( OI.w == 0x4300 )//操作电气设备
    {
        eResultChoice = ActionRequestMeter( Ch,  pOMD);
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

    if( DataLen == 0x8000 )//如果数据解析错误，不进行后续List的解析
    {
        return eAPP_RETURN_DATA;
    }

    APPData[Ch].BufDealLen.w += (DataLen+4);

    return eResultChoice;
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
    eAPPBufResultChoice     eResultChoice;

    if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return;
    }

    DLT698AddBuf(Ch, 0, LinkData[Ch].pAPP,3);//添加命令头
    APPData[Ch].BufDealLen.w = 3;
    eResultChoice = eAPP_ERR_RESPONSE;

    switch(pAPDU[1])
    {
    case ACTION_REQUEST_NORMAL:
        eResultChoice = ActionRequest( Ch,(BYTE  *)&(pAPDU[3]) );
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

static WORD AnalyseGetRequestRecordLen( BYTE *pOAD )
{
    WORD Len,Len1;
    
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
    
	Len = 3;

	switch(pAPDU[1])
	{		
	case GET_REQUEST_NORMAL:
		Len += 4;
		break;
	case GET_REQUEST_NORMALLIST:	
		Len += pAPDU[3]*4+1;
		break;
	case GET_REQUEST_RECORD:
        Len1 = AnalyseGetRequestRecordLen( &pAPDU[Len] );
        if( Len1 == 0x8000 )
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
		case GET_RESPONSE:
			//模块要求解析数据@@@@--未完成lzq
			Len = 0;
			break;
		case SECURITY_REQUEST:
            return 0x8000;	
		default:
		    return 0x8000;
		}
	}

	return Len;
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
    if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return;
    }

    DLT698AddOneBuf(Ch,0x77, 00);//添加00 无主动上报
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
static void ProcDlt698Apdu( BYTE Ch)
{
    if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return;
    }
    
	if( APPData[Ch].eSafeMode != eSECURITY_FAIL )//解密失败不进行处理
	{
		switch( LinkData[Ch].pAPP[0] ) //cmd
		{
		case GET_REQUEST:
			DealGet_Request( Ch,&LinkData[Ch].pAPP[0] );
			break;	
		case SECURITY_REQUEST:
			APPData[Ch].APPFlag = APP_ERR_DATA;
			break;
		case SET_REQUEST:
			DealSet_Request(Ch, &LinkData[Ch].pAPP[0]);
			break;
		case GET_RESPONSE:
		    DealGet_Response( Ch, &LinkData[Ch].pAPP[0] );
		    break;
        case ACTION_REQUEST:
            DealAction_Request( Ch,&LinkData[Ch].pAPP[0] );
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
static void InitDlt698APP( BYTE Ch)
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
	DWORD Result;
	
	if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return;
    }
    
 	if( LinkData[Ch].eLinkDataFlag == eAPP_OK )//链路层数据准备好
 	{
 	    memset( &APPData[Ch].APPFlag, 0x00, sizeof(TDLT698APPData));//初始化应用层结构体
 		
	 	Result = JudgeTimeTagValid( Ch );//解密数据后进行时间标签判断 加密数据中包含时间标签

	 	InitDlt698APP(Ch);//对应用层结构体等进行初始化

 		if( Result == FALSE )//解析apdu出现错误
 		{
            DealErrowResponse( Ch );
 		}
 		else
 		{            
            ProcDlt698Apdu(Ch);//进行apdu的处理
 		}

		ProcFollowReportAndTimeFlag( Ch);//添加时间标签和主动上报
    
		ProFollowJudge(Ch);//链路层分帧判断
 	}
}
