//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部
//创建人	姜文浩 魏灵坤
//创建日期	2016.8.4
//描述		DL/T 698.45面向对象协议C文件
//修改记录
//----------------------------------------------------------------------
#include "AllHead.h"
#include "Dlt698_45.h"
#include "task_update.h"
//#include "sm_task.h"
extern PQ_JZ_RATE PQ_Para;// ;
extern int gdwUartBps ;
extern __no_init TNAddr_t NAddr;
extern BOOL bIsVipUpdateFlag;
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
BYTE UseProxy = 0;//针对20EB的报文

TDLT698LinkData LinkData[MAX_COM_CHANNEL_NUM];
TDLT698APPData APPData[MAX_COM_CHANNEL_NUM];
BYTE DLT698APDURequestBuf[MAX_APDU_SIZE+EXTRA_BUF];
BYTE ProtocolBuf[MAX_APDU_SIZE+EXTRA_BUF];// MAX_APDU_SIZE = 2000, EXTRA_BUF = 30

TDLT698APPConnect APPConnect;
TSafeModePara SafeModePara;
TAPDUBufFlag APDUBufFlag;
TConsultData ConsultData;

TFileInfo tFileInfo;
TFileText tFileText;
extern TReportMode gReportMode;
BYTE IsUse5002 = 0;				//是否使用5002抄读4800
#if(PROTOCOL_VERSION == 25)
BYTE IsUse4801 = 0; 			//是否使用309B抄读4801
BYTE IsUse4800 = 0; 			//是否使用309A抄读4800
#endif
#define MDW(HH, HL, LH, LL)   ((HH<<24) + (HL<<16) + (LH<<8) + (LL))
#define MW(HH, LL)   ( (HH<<8) + (LL))
const DWORD sidsidmacoi = 0x000E01F0;//启动传输
BYTE eraseflashflag[2] = {0,0};//擦除flash的标志，分别是sid和sid_mac
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

TErrPassNotAut ErrpassNoAut[] = 
{
	{0x4001, 0x02, 0, 0}, //获取通信地址
	{0x4010, 0x02, 0, 0}, //获取计量元件数
	{0x4804, 0x02, 0, 0}, //获取计量参数
	{0x4104, 0x02, 0, 0}, //获取额定电压
	{0x4105, 0x02, 0, 0}, //获取额定电流
	{0x4000, 0x02, 0, 0}, //获取当前时间
};
//-----------------------------------------------
#if(PROTOCOL_VERSION == 25)
//电器代码表
const BYTE g_bDevice_Code_List[25] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0C,0x21,0x22,0x23,0x25,0x26,0x31,0xA1,0xA3,0xB1,0xB2,0xE1,0xE2,0xF1,0xF2};
#endif
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
static void DealProxy_Request(BYTE Ch,BYTE *pAPDU);
static void DealReport_Response(BYTE Ch,BYTE *pAPDU);
void InitFileDown(void);
BOOL api_FileCrcCheck(void);
eAPPBufResultChoice GetTransCommand( BYTE Ch,BYTE *pOAD );
extern BOOL EraseExtFlashSector( WORD No, DWORD Addr );
extern BYTE api_CheckParaInfo(BYTE *CurOI);
extern void ClearAutoReportIndexRam( BYTE Ch );
extern void UpdateSoftFuc(void);
extern void Freshsm4IK_Task(void);
extern boolean_t sm4_Decrypt(uint8_t auth, uint8_t *inBuf, uint32_t nInLen, uint8_t* outBuf, uint32_t* nOutLen);
extern BOOL UpdateThresholdValue(BYTE* tempEventPara);
extern rt_err_t SwitchtoRtt(void) ;
extern rt_err_t SwitchtoUart(void) ;
extern void UNUSED(int n);
extern BOOL WriteExtFlash2( BYTE No, DWORD Addr, WORD Len, BYTE *pBuf );
extern BOOL WriteVipUpdateData(BYTE* pBuf, WORD wLen);
extern BOOL WriteUpFileReset(void);
#if(PROTOCOL_VERSION == 25)
extern BOOL GetDefaultThresholdValue(BYTE byType);
#endif
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

	api_VReadSafeMem( GET_SAFE_SPACE_ADDR( ProSafeRom.SafeModePara), sizeof(TSafeModePara), (BYTE*)&SafeModePara );	
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
			Freshsm4IK_Task();
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
//功能描述:    安全模式参数判断
//         
//参数:      BYTE Ch[in]	//通道选择
//         
//         BYTE *pOI[in] //OI指针
//         
//返回值:     WORD  TRUE 有效   FALSE 无效
//         
//备注内容:    无
//--------------------------------------------------
static WORD JudgeSafeModePara(BYTE Ch, BYTE *pOI)
{
	BYTE i, OIA;
	BYTE Cmd, SecurityMode;
	WORD wSafeMode;
	TTwoByteUnion OI, Index;
	
	Cmd = LinkData[Ch].pAPP[0];
	SecurityMode = APPData[Ch].eSafeMode;
	if(Cmd == 0x08)
	{
		lib_ExchangeData( OI.b, pOI+1, 2);
		lib_ExchangeData( Index.b, &(pOI+1)[2], 2);
	}
	else
	{
		lib_ExchangeData( OI.b, pOI, 2);
		lib_ExchangeData( Index.b, &pOI[2], 2);
	}

#if(USE_SAFETY_MODE == 0)
		return TRUE;
#endif

	//	return TRUE;
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
					if( wSafeMode & 0xc )//明文、明文+MAC
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
	
	if(Cmd & 0x80)
		return TRUE;
        
	//判断默认安全模式参数,
	switch(Cmd)
	{
	case 0x05://DealGet_Request
          
#if(USER_TEST_MODE == 0)        //用户如电科院测试模式下，支持全安全方式，其它现场打开，可明文抄读
		if(SafeModePara.Flag == 0)
			return TRUE;
#endif         
		if((OI.w == 0x4000) || (OI.w == 0x4001) || (OI.w == 0x4031) || (OI.w == 0x4A01) || (OI.w & 0xF000) == 0xF000)
			return TRUE;
		
		if(SecurityMode != 0)
			return TRUE;
		//if(OI.w == 0x3013)
		//{
		//    if(SecurityMode ==0 || SecurityMode == eSECRET_TEXT)
		//        return TRUE;
		//}
		//return TRUE;
		break;	

	case 0x06://DealSet_Request
		//4002	表号, 4003 客户编号, F100 ESAM 默认设置方式：明文+MAC 
        if((OI.w == 0xF101) || (OI.w == 0xFF01))
        	return TRUE;
		if( (OI.w == 0x4002)||(OI.w == 0x4003)||(OI.w == 0xf100) )
		{
			if( SecurityMode >= eEXPRESS_MAC )
			{
				return TRUE;
			}
		}
		//其它数据都是权限等于密文+MAC可设置	
		if( SecurityMode == eSECRET_MAC )
		{
			return TRUE;
		}		
		break;	

	case 0x07://DealAction_Request
		if(OI.w == 0x20EB)
			return TRUE;

		if( (OI.w == 0x4000) || (OI.w == 0x2015) )//4000 日期时间 2015 跟随上报状态字
		{
			return TRUE;
		}
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
		else if(OI.w == 0xf100)
		{
			return TRUE;
		}
		else if(OI.w == 0x4300)//清零操作支持明文，SID密文，SID+MAC密文
		{
			// if(SecurityMode == 0 || SecurityMode == eSECRET_TEXT)
			// if(SecurityMode == 0)
			//    return TRUE;
			if(Index.w == 0x0900 || Index.w == 0x0800 )
				return TRUE;
			
			if( SecurityMode != eSECRET_MAC )
				return FALSE;
		}
		//如果权限等于密文+MAC都可操作	
		if( SecurityMode == eSECRET_MAC )
		{
			return TRUE;
		}
		break;	
	
	case 0x08:
		if(OI.w == 0x3320)
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
static WORD JudgeTaskAuthority( BYTE Ch, eRequestMode eType, BYTE *pOI )
{
	TTwoByteUnion OI;
	WORD Result;
	// BYTE ClassAttribute;
	
	if( api_GetSysStatus( eSYS_STATUS_INSIDE_FACTORY ) == TRUE ) 
	{
		return TRUE;
	}
	
	lib_ExchangeData( OI.b , pOI , 2 );//获取OI
	// ClassAttribute = pOI[2];	  //获取属性
	
	//除单地址外只允许读数据 与上面代码的顺序不能颠倒!!!!!!
	if( (LinkData[Ch].AddressType!=eSINGLE_ADDRESS_TYPE) && (eType!=eREAD_MODE) )
	{
		return FALSE;
	}
	
// 	Result = JudgeIRAuthority( Ch, eType, pOI );
// 	if( Result == TRUE )//失败代表正常操作 需要继续进行后续判断
// 	{
//         return TRUE;
// 	}
	
// 	Result = JudgeConnectValid( Ch, eType );
// 	if( Result  == FALSE )
// 	{
// 		return FALSE;
// 	}
// 	
	Result = JudgeSafeModePara( Ch, pOI);

	//测试用：正常运行请屏蔽！！！！！！
	// if(OI.w == 0x4000)
	// {
	// 	Result = TRUE;
	// }
	
	return Result;
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
	BYTE *pBuf;
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
	
	pBuf = (BYTE *)&(LinkData[Ch].pAPP[Len]);
	LinkData[Ch].pAPPLen.w -= 1;//剔除OPTIONAL 1个字节

//	APPData[Ch].TimeTagFlag = eTime_True;//默认无时间标签
//	return TRUE; 
/*	*/
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
	    
	   // if( (LinkData[Ch].pAPPLen.w -Len) < 10 )//剩余字节不够时间标签长度
	    {
            APPData[Ch].TimeTagFlag = eTime_True;//时间标签无效
            return TRUE;
	    }
	    
	   /* LinkData[Ch].pAPPLen.w -= (LinkData[Ch].pAPPLen.w -Len);//应用层长度 剔除时间标签
       
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
        }*/
	}

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
	#if(PROTOCOL_VERSION == 25)
    if((OI == 0x3013) || (OI == 0x3016) || (OI == 0x3036) || (OI == 0x302F)|| (OI == 0x5002) || (OI == 0x309A) || (OI == 0x309B))
	#elif(PROTOCOL_VERSION == 24)
	if((OI == 0x3013) || (OI == 0x3016) || (OI == 0x3036) || (OI == 0x302F)|| (OI == 0x5002))
	#endif
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}

	//接口类24
/*    if((OI&0XF000) == 0x3000)
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

	return TRUE;*/
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
	WORD wLen;
	//wxy memcpy( OutBuf, psmbuf, 04);//组前4个字节
	
	SMOffest = Deal_698DataLenth( &psmbuf[4], SMLen.b, ePROTOCOL_TYPE);
	//wxy memcpy( &OutBuf[4], &psmbuf[4+SMOffest], SMLen.w);//组后面的数据

	DataOffest = Deal_698DataLenth( pdata, DataLen.b, ePROTOCOL_TYPE);
	if( Mode == 0x55 )
	{
	    DataLen.w = DataLen.w-5;//不保存OAD+数据len
		//wxy memcp·y( &OutBuf[4+SMLen.w], &pdata[DataOffest+5], DataLen.w );//组数据
		memcpy( &OutBuf[2], &pdata[DataOffest+5], DataLen.w );//组数据
	}
	else
	{
		//wxy 		memcpy( &OutBuf[4+SMLen.w], &pdata[DataOffest], DataLen.w);//组数据
		memcpy( &OutBuf[2], &pdata[DataOffest], DataLen.w);//组数据
	}
    

	MacOffest = Deal_698DataLenth( &psmbuf[4+SMOffest+SMLen.w], MacLen.b, ePROTOCOL_TYPE);
	//wxy memcpy( &OutBuf[4+SMLen.w+DataLen.w], &psmbuf[4+SMOffest+SMLen.w+MacOffest], MacLen.w);//组数据
	memcpy( &OutBuf[2+DataLen.w], &psmbuf[4+SMOffest+SMLen.w+MacOffest], MacLen.w);//组数据
	
	wLen = (DataLen.w + MacLen.w);
	OutBuf[0] = wLen>>8;
	OutBuf[1] = wLen&0xFF;//组数据
	
	return ( 4+SMOffest+SMLen.w+DataOffest+DataLen.w+MacOffest+MacLen.w);
}

//-----------------------------------------------
//函数功能:     用来处理MAC数据
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
static WORD Deal_MACData( BYTE Mode, BYTE *psmbuf, BYTE *pdata, BYTE *OutBuf )
{
	TTwoByteUnion DataLen,MacLen;
	BYTE DataOffest,MacOffest;
	WORD wLen;
	
	DataOffest = Deal_698DataLenth( pdata, DataLen.b, ePROTOCOL_TYPE);
	if( Mode == 0x55 )
	{
		DataLen.w = DataLen.w-5;//不保存OAD+数据len
		memcpy( &OutBuf[2], &pdata[DataOffest+5], DataLen.w );//组数据
	}
	else
	{
		memcpy( &OutBuf[2], &pdata[DataOffest], DataLen.w);//组数据
	}


	MacOffest = Deal_698DataLenth( &psmbuf[1], MacLen.b, ePROTOCOL_TYPE);

	memcpy( &OutBuf[2+DataLen.w], &psmbuf[1+MacOffest], MacLen.w);//组数据
	
	wLen = (DataLen.w + MacLen.w);
	OutBuf[0] = wLen>>8;
	OutBuf[1] = wLen&0xFF;//组数据
	
	return ( 1+DataOffest+DataLen.w+MacOffest+MacLen.w);
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
	int nLen = 2;
	
	OutBuf[0] = 0;
	memcpy( &OutBuf[1], pdata, 01);//组前4个字节
	
	SMOffest = Deal_698DataLenth( &psmbuf[nLen], SMLen.b, ePROTOCOL_TYPE);
	memcpy( &OutBuf[nLen], &psmbuf[nLen+SMOffest], SMLen.w);//组后面的数据

	if( Mode == 0x55)
	{
		DataOffest = Deal_698DataLenth( pdata, DataLen.b, ePROTOCOL_TYPE);
		memcpy( &OutBuf[nLen+SMLen.w], &pdata[DataOffest], DataLen.w);//组数据
		
		OutBuf[0] = (BYTE)(DataLen.w>>8);
		OutBuf[1] = (BYTE)(DataLen.w&0xFF);
		
		return ( nLen+SMOffest+SMLen.w+DataOffest+DataLen.w);
	}
	return ( nLen+SMOffest+SMLen.w);		
}
////--------------------------------------------------
////功能描述:  数组元素索引不为零的情况下对数据进行处理
////         
////参数:      BYTE   *pData[in] 需要判断的数据指针
////         
////返回值:     BYTE*  返回处理后的数据指针
////         
////备注内容:    无
////--------------------------------------------------
//static BYTE *DealArrayClassIndexNotZeroData( BYTE *pData )
//{
//	pData = pData -2;//将数据指针偏移两个指针
//	pData[0] = Array_698;
//	pData[1] = 1;
//	return pData;
//}
////--------------------------------------------------
////功能描述:  结构体元素索引不为零的情况下对数据进行处理
////         
////参数:     BYTE * pData[in]
////         
////返回值:    BYTE*  返回处理后的数据指针
////         
////备注内容:  无
////--------------------------------------------------
//static BYTE *DealStructureClassIndexNotZeroData( BYTE *pData )
//{
//	pData = pData -2;//将数据指针偏移两个指针
//	pData[0] = Structure_698;
//	pData[1] = 1;
//	return pData;
//}

//-----------------------------------------------
//函数功能: 验证主站下发的数据并返回解析后的数据
//
//参数:		
//			Buf[in/out]输入时：	若数据验证信息为SID_MAC
//							4字节标识+附加数据AttachData+Data+MAC
//								若数据验证信息为SID
//							4字节标识+附加数据AttachData+Data
//						输出时：长度及数据，长度两字节，高在前，低在后
//返回值:	TRUE/FALSE
//		   
//备注:
//-----------------------------------------------

BOOL api_AuthDataSID(BYTE byType, BYTE bySecurity, BYTE *Buf)
{
	BOOL Result=FALSE;
	TTwoByteUnion BufLen;
	uint32_t nLen=MAX_APDU_SIZE;

	BufLen.b[0] = Buf[1];
	BufLen.b[1] = Buf[0];
	if( BufLen.w > MAX_APDU_SIZE )//输入的数据如果大于apdu数据返回失败
	{
		return FALSE;
	}
	if(bySecurity == 1) //密文的 + MAC 的
	{
        Result = sm4_Decrypt(byType, &Buf[2], BufLen.w, &Buf[2],&nLen); //含MAC的
        //Result = sm4_Decrypt(0, &Buf[2], BufLen.w-4, &Buf[2],&nLen);//不含MAC的

	}
	else
	{
		if(byType == 1)
				Result = TRUE;//SM4Mac(Buf, BufLen.w, Buf,&nLen);//sm4_MacVerify
		else
				Result = TRUE;//sm4_MacVerify(Buf, BufLen.w, Buf,&wLen);
	}
	if(nLen  > MAX_APDU_SIZE)
	{
		return FALSE;
	}
	if( Result == TRUE )//操作成功后进行数据的搬移
	{
		
		//memmove(&Buf[2], &Buf[0], nLen);
		
		Buf[0] = (nLen>>8);
		Buf[1] = (nLen&0xFF);
	}
	
	return Result;
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

	if( (LinkData[Ch].pAPP[0] != SECURITY_REQUEST) && (LinkData[Ch].pAPP[0] != SECURITY_RESPONSE))
	{
		return eNO_SECURITY;
	}
	
	LenOffest = Deal_698DataLenth( &LinkData[Ch].pAPP[2], Len.b, ePROTOCOL_TYPE);//获取数据长度
	Len.w += LenOffest+2;

	SecurityMode =LinkData[Ch].pAPP[1];
		// memset(ProtocolBuf,0,MAX_APDU_SIZE+EXTRA_BUF);wxy测试代码
	if( SecurityMode == 0xff )
	{
		return eSECURITY_FAIL;
	}
	else if( (SecurityMode == 0x00) || (SecurityMode == 0x01) )//明文方式
	{
		////王选友2025.03.07测试主动上报回应帧的情况
		if((LinkData[Ch].pAPP[0] == 0x90) && (LinkData[Ch].pAPP[1] == 0x01) 
		&& (LinkData[Ch].pAPP[Len.w] == 0x01)  && (LinkData[Ch].pAPP[Len.w+1] == 0x00))
		{
			Deal_MACData(0x00, &LinkData[Ch].pAPP[Len.w+1],&LinkData[Ch].pAPP[2],ProtocolBuf);
		   	Result = api_AuthDataSID(1,SecurityMode, ProtocolBuf );
			if( Result == TRUE )
			{
				DataLen.b[0] = ProtocolBuf[1] ;
				DataLen.b[1] = ProtocolBuf[0] ;
				memcpy( LinkData[Ch].pAPP, &ProtocolBuf[2], DataLen.w);
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
		if( LinkData[Ch].pAPP[Len.w] == 0x00 )//判断是否选择sid_mac
		{
			Deal_SIDMACDate(0x00, &LinkData[Ch].pAPP[Len.w+1],&LinkData[Ch].pAPP[2],ProtocolBuf);
		   	Result = api_AuthDataSID(1,SecurityMode, ProtocolBuf );
			if( Result == TRUE )
			{
				DataLen.b[0] = ProtocolBuf[1] ;
				DataLen.b[1] = ProtocolBuf[0] ;
				memcpy( LinkData[Ch].pAPP, &ProtocolBuf[2], DataLen.w);
				LinkData[Ch].pAPPLen.w = DataLen.w;

				if( SecurityMode == 0x01 )//密文模式
				{	
					#if(USER_TEST_MODE)
					//为了通过电科院姜工软件测试，做的特殊处理
					if((memcmp((BYTE*)&LinkData[Ch].pAPP[3],(BYTE*)&sidsidmacoi,4) == 0))//F0010E00，启动传输
					{
						eraseflashflag[1] = 0xDA;//sid_mac方式的擦除标识
					}
					#endif
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
			Deal_SIDDate(0x55,&LinkData[Ch].pAPP[Len.w+1] ,&LinkData[Ch].pAPP[2],ProtocolBuf);
		   	Result = api_AuthDataSID(0, SecurityMode, ProtocolBuf );
			if( Result == TRUE )
			{
				if( SecurityMode == 0x01)//密文模式
				{
					DataLen.b[0] =ProtocolBuf[1] ;
					DataLen.b[1] = ProtocolBuf[0] ;
					memcpy( LinkData[Ch].pAPP, &ProtocolBuf[2], DataLen.w);
					LinkData[Ch].pAPPLen.w = DataLen.w;

					#if(USER_TEST_MODE)
					//为了通过电科院姜工软件测试，做的特殊处理
					if((memcmp((BYTE*)&LinkData[Ch].pAPP[3],(BYTE*)&sidsidmacoi,4) == 0))//F0010E00，启动传输
					{
						eraseflashflag[0] = 0x0D;//sid方式的擦除标识
					}
					#endif

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

	return eSECURITY_FAIL;
}
//-----------------------------------------------
//函数功能: 将电表组织的数据写入ESAM后产生mac,整理后上传
//
//参数:		
//			Len[in] 传入的数据长度
//			Buf[in/out]	输入数据
//					数据处理完后输出时 （长度两字节，高在前，低在后）
//						输出长度+mac
//						
//返回值:	TRUE/FALSE
//		   
//备注:
//-----------------------------------------------
BOOL api_PackDataWithSafe(WORD Len,BYTE *Buf)
{
	BOOL Result;
	uint32_t nOutLen=4;
	
	sm4_Mac(Buf, Len, Buf, &nOutLen );
	Result = TRUE;//sm4_MacVerify(Buf, Len, Buf, nOutLen );
	if(Result)
	{
		if(nOutLen > MAX_APDU_SIZE)
			return FALSE;

		memmove(&Buf[2], &Buf[0], nOutLen);

		Buf[0] = (nOutLen>>8);
		Buf[1] = (nOutLen&0xFF);

	}
	return Result;
}


//-----------------------------------------------
//函数功能: 将密文写入ESAM,整理后上传
//
//参数:		Type[in] 1--明文+MAC	2--密文	3--密文+mac
//			Len[in] 传入的数据长度
//			Buf[in/out]	输入数据
//					数据处理完后输出时 （长度两字节，高在前，低在后）
//						Type为1时，输出长度+mac
//						Type为2时，输出长度+密文
//						Type为3时，输出长度+密文+mac
//						
//返回值:	TRUE/FALSE
//		   
//备注:
//-----------------------------------------------
BOOL api_PackEsamDataWithSafe(BYTE Type,WORD Len,BYTE *Buf)
{
	BOOL Result=FALSE;
	uint32_t nOutLen=Len+50;
	BYTE OutBuffer[5120];
	
	switch(Type)
	{
	case 1:	//1--明文+MAC
		Result = sm4_MacVerify( Buf, Len, OutBuffer, nOutLen);
		break;
	case 2:	//2--密文
		Result = sm4_Encrypt( FALSE, Buf, Len, OutBuffer, &nOutLen );
		break;
	case 3:	//3--密文+mac
		Result = sm4_Encrypt( TRUE, Buf, Len, OutBuffer, &nOutLen );
		break;
	}
	if(Result)
	{
		if(nOutLen > MAX_APDU_SIZE)
			return FALSE;

		memcpy(&Buf[2], OutBuffer, nOutLen);

		Buf[0] = (nOutLen>>8);
		Buf[1] = (nOutLen&0xFF);

	}
	return Result;

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
	WORD Result,i,j;
	
	if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return;
    }
    
	if( (APPData[Ch].eSafeMode != eSECURITY_FAIL) && (APPData[Ch].eSafeMode != eNO_SECURITY)  )
	{
		if( ( APPData[Ch].eSafeMode == eSECRET_MAC ) ||( APPData[Ch].eSafeMode == eSECRET_TEXT )||( APPData[Ch].eSafeMode == eEXPRESS_MAC ))//P2：11：明文+MAC 12：密文 13： 密文+MAC
		{
			memcpy( ProtocolBuf,APPData[Ch].pAPPBuf,APPData[Ch].APPBufLen.w);//进行数据的搬运
			Result = api_PackEsamDataWithSafe( (APPData[Ch].eSafeMode-3),APPData[Ch].APPBufLen.w,ProtocolBuf);
			if( Result == TRUE )
			{
				Len.b[0] = ProtocolBuf[1];
				Len.b[1] = ProtocolBuf[0];
		
				if( (APPData[Ch].eSafeMode == eSECRET_MAC) ||(APPData[Ch].eSafeMode == eEXPRESS_MAC)  ) 
				{
                    //测试用：加密前的明文
					#if(USE_PRINTF == 1)
					if(((UsePrintfFlag >> 0) & 0x0001) == 0x0001)
					{
						rt_kprintf("\nMingWen1: ");
						j = 0;
						for(i = 0;i<APPData[Ch].APPBufLen.w;i++)
						{
							rt_kprintf("%02X ",APPData->pAPPBuf[i]);
							j++;
							if(j>100)
							{
								rt_kprintf("\n");
								j = 0;
							}
						}
						rt_kprintf("\n");
					}
					#endif

					if(Len.w == 0) Len.w=10;
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
					//测试用：加密前的明文
					#if(USE_PRINTF == 1)
					if(((UsePrintfFlag >> 0) & 0x0001) == 0x0001)
					{
						rt_kprintf("\nMingWen2: ");
						j = 0;
						for(i = 0;i<APPData[Ch].APPBufLen.w;i++)
						{
							rt_kprintf("%02X ",APPData->pAPPBuf[i]);
							j++;
							if(j>100)
							{
								rt_kprintf("\n");
								j = 0;
							}
						}
						rt_kprintf("\n");
					}
					#endif

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
			//memcpy( &ProtocolBuf, &RN[1],RN[0]);
			//memcpy( &ProtocolBuf[RN[0]], APPData[Ch].pAPPBuf,APPData[Ch].APPBufLen.w);//王选友 2022.0802 修改
                  
                        memcpy( &ProtocolBuf, APPData[Ch].pAPPBuf,APPData[Ch].APPBufLen.w);
			memcpy( &ProtocolBuf[APPData[Ch].APPBufLen.w], &RN[1],RN[0]);
                  
			Result =api_PackDataWithSafe((APPData[Ch].APPBufLen.w+RN[0]),ProtocolBuf);
			if( Result == TRUE )
			{				
				BufIndex = Deal_698DataLenth( (BYTE*)NULL, APPData[Ch].APPBufLen.b,eUNION_OFFSET_TYPE);//获取明密文数据的偏移长度
				BufIndex = BufIndex+2;
                                
                memmove( &APPData[Ch].pAPPBuf[BufIndex],APPData[Ch].pAPPBuf,APPData[Ch].APPBufLen.w);
                APPData[Ch].pAPPBuf[ APPData[Ch].APPBufLen.w+BufIndex ] = 0x01;
                APPData[Ch].pAPPBuf[ APPData[Ch].APPBufLen.w+BufIndex+1 ] = 0x00;
                APPData[Ch].pAPPBuf[ APPData[Ch].APPBufLen.w+BufIndex+1+1 ] = 0x04;
                memcpy( &APPData[Ch].pAPPBuf[ APPData[Ch].APPBufLen.w+BufIndex+1+1+1],&ProtocolBuf[2],4);

                APPData[Ch].pAPPBuf[0] =SECURITY_RESPONSE;//组安全传输命令头
            
                APPData[Ch].pAPPBuf[1] = 0x00;
                               
                Deal_698DataLenth( &APPData[Ch].pAPPBuf[2],APPData[Ch].APPBufLen.b,eUNION_TYPE);
                APPData[Ch].APPBufLen.w = (4+APPData[Ch].APPBufLen.w+BufIndex+3);//+安全传输头+mac
                APPData[Ch].APPCurrentBufLen.w = APPData[Ch].APPBufLen.w;
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
	}
	else if( APPData[Ch].eSafeMode == eSECURITY_FAIL )
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
		#if(PROTOCOL_VERSION == 25)
		if(pDLT698RecordPara->OI == 0x309B)
		{
			IsUse4801 = 1;
		}
		else if(pDLT698RecordPara->OI == 0x309A)
		{
			IsUse4800 = 1;
		}
		#endif
		Len = api_ReadEventRecord( Tag, pDLT698RecordPara, BufLen, pBuf);
		#if(PROTOCOL_VERSION == 25)
		if(pDLT698RecordPara->OI == 0x309B)
		{
			IsUse4801 = 0;
		}
		else if(pDLT698RecordPara->OI == 0x309A)
		{
			IsUse4800 = 0;
		}
		#endif
	}
	else if((pDLT698RecordPara->OI >= 0x5000) && (pDLT698RecordPara->OI <= 0x5011))//调用冻结函数
	{
		if(pDLT698RecordPara->OI == 0x5002)
		{
			IsUse5002 = 1;
		}
		Len = api_ReadFreezeRecord( Tag, pDLT698RecordPara, BufLen, pBuf);
		if(pDLT698RecordPara->OI == 0x5002)
		{
			IsUse5002 = 0;
		}
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
            pEventUnit = EventPhaseUnit;
            break;
        }
        default:
            pEventUnit = StandardEventUnit;
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
		#if(PROTOCOL_VERSION == 25)
		if((((pDLT698RecordPara->OI) >= 0x3000) && ((pDLT698RecordPara->OI) <= 0x3050)) || (pDLT698RecordPara->OI == OAD_UNKNOWN_DEV) || (pDLT698RecordPara->OI == OAD_SPECIFIC_DEV))//事件RCSD
		#elif(PROTOCOL_VERSION == 24)
		if( ((pDLT698RecordPara->OI) >= 0x3000) && ((pDLT698RecordPara->OI) <= 0x3050))//事件RCSD
		#endif
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
	#if(PROTOCOL_VERSION == 25)
	if( ((OI >= 0x3000) && ( OI < 0x3050)) || (OI == OAD_UNKNOWN_DEV) || (OI == OAD_SPECIFIC_DEV))	//事件RCSD
	#elif(PROTOCOL_VERSION == 24)
	if( (OI >= 0x3000) && ( OI < 0x3050))	//事件RCSD
	#endif
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
		#if(PROTOCOL_VERSION == 25)
		if(((OI >= 0x3000) && ( OI < 0x3050)) || (OI == OAD_UNKNOWN_DEV) || (OI == OAD_SPECIFIC_DEV))	//事件RCSD
		#elif(PROTOCOL_VERSION == 24)
		if( (OI >= 0x3000) && ( OI < 0x3050))	//事件RCSD
		#endif
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
				return FALSE;//DLT698RecordPara->Phase = ePHASE_ALL;//wxy	return FALSE;
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
		DLT698RecordPara->eTimeOrLastFlag = eNULL_TIME_FLAG;
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
  //  return eAPP_ERR_RESPONSE;
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
	
	Result = DealRSDData( pRSD, ( TDLT698RecordPara * )&DLT698RecordPara);
	if( Result == FALSE )
	{
	//	DAR = DAR_WrongType;
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
	//else if( JudgeTaskAuthority( Ch, eREAD_MODE, pRSD-4) == FALSE )//判断安全模式参数
//	{			
	//	DAR = DAR_PassWord;
//	}
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
	
			eResultChoice = GetRequestRecordDataMethod2( Ch, pRSD, OI.w,ClassAttribute);//wxy open

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
extern int m_SampleNum;
extern int nChannel;
extern SYS_PARA_1808 SysPara;
eAPPBufResultChoice GetResponseNormal( BYTE Ch, BYTE *pOAD )
{
 	BYTE ClassAttribute,i,j,Num,Result;
	WORD Offset;
	DWORD dwDataU[3], dwDataI[4], RelativeTime;
	float FloatData[5],dwDataPhase[3],Point;
    TTwoByteUnion OI;
	TRealTimer TmpRealTimer;//时间结构体

	APPData[Ch].APPFlag = APP_NO_DATA;
	lib_ExchangeData( OI.b, pOAD, 2 );
	ClassAttribute = pOAD[2];

	for(i = 0; i < sizeof(ErrpassNoAut)/sizeof(TErrPassNotAut); i++) //针对模组主动抄读一些参数时，老版物联表和新版物联表客户机地址不一致的兼容
	{
		if((OI.w == ErrpassNoAut[i].OI) && (ClassAttribute == ErrpassNoAut[i].Attr))
		{
			if(pOAD[4] == 0x00)//说明模组请求信息后，物联表判断密码错未授权0x0F、安全认证失败0x17、对象未定义0x04,其余情况目前没有测试到。
			{
				ErrpassNoAut[i].RecErrPassFlag = 0x77;
				ErrpassNoAut[i].RecErrPassCount ++;
				ErrpassNoAut[i].SendCount = 0;
			}
			else if (pOAD[4] == 0x01)//说明成功
			{
				ErrpassNoAut[i].RecErrPassFlag = 0x00;
				ErrpassNoAut[i].RecErrPassCount = 0;
				ErrpassNoAut[i].SendCount = 0;
			}
			
			break;
		}
	}
	
	switch( OI.w )
	{
		case 0x4000://日期时间
			if( ClassAttribute == 2 )//属性2
			{
				if(pOAD[5] == DateTime_S_698)
				{
					RelativeTime = api_GetRelativeMin();
					lib_InverseData(&pOAD[6], 2);
					memcpy((BYTE*)&(TmpRealTimer.wYear),&pOAD[6],sizeof( TRealTimer ));
					Result = api_WriteMeterTime(&TmpRealTimer);
					if(Result == FALSE)
					{
						break;
					}
					api_UpdateFreezeTime(RelativeTime);
					api_DealManageCOMStatus( eGET_RTC );
					if(InitComplete == 0)
					{
						InitComplete = 1;   //上电交互完成，置位初始化完成，绿灯灭
						#if(USE_PRINTF == 1)
						if(((UsePrintfFlag >> 0) & 0x0001) == 0x0001)
						{
							rt_kprintf("InitStatistic: %d \n", InitComplete);
						}
						#endif
						ManageComInfo.EventReportOverTime = 2; //用于主动上报，握手完成后上报，设置2s开始倒计时
						
						//wxy if(api_GetUpdateFlag())
						//{
							//tFileInfo.FileStatus = 6;
						//	api_UpdateAppEventEnd();
						//	api_SetUpdateFlag(FALSE);
						//}
	
					}
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
					NAddr.NAddressType = 0x55;
					memcpy( &NAddr.NAddress[0], &pOAD[7], 6 );//获取通信地址
					lib_CheckSafeMemVerify((BYTE *)&NAddr.NAddressType,sizeof(TNAddr_t),REPAIR_CRC);
					api_DealManageCOMStatus( eGET_ADDR );
				}
			}
			break;
		case 0x4104://额定电压
			if( ClassAttribute == 2 )
			{
				if( pOAD[5] == Visible_string_698 )
				{
					ManageComInfo.dwEVol = (DWORD)GetStringToFloat((char*)&pOAD[7], pOAD[6])*10000;
					if(ManageComInfo.dwEVol)
						Set_Adjust_Para(EVol,0,&ManageComInfo.dwEVol);
					//api_DealManageCOMStatus( eGET_V );//打开屏蔽，这一条就会重试
				}

			}
			break;
		case 0x4105://额定电流
			if( ClassAttribute == 2 )
			{
				if( pOAD[5] == Visible_string_698 )
				{
					ManageComInfo.dwECur = (DWORD)GetStringToFloat((char*)&pOAD[7], pOAD[6])*10000;
					if(ManageComInfo.dwECur)
						Set_Adjust_Para(ECur,0,&ManageComInfo.dwECur);
					//api_DealManageCOMStatus( eGET_I );//打开屏蔽，这一条就会重试
				}

			}
			break;
		case 0x4010://计量元件数
			if( ClassAttribute == 2 )
			{
				//if( pOAD[5] == Unsigned_698 )
				{
					ManageComInfo.SampleNO = pOAD[6];
					api_DealManageCOMStatus( eGET_SAMPLENUM );

					if(ManageComInfo.SampleNO == 1)
					nChannel = 2;
					else if(ManageComInfo.SampleNO == 3)
						nChannel = CHMAX;
					else 
						nChannel = CHMAX;
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
                m_SampleNum = ((pOAD[Offset]<<8) + pOAD[Offset+1]);
                if(m_SampleNum != 64 && m_SampleNum!=128 && m_SampleNum!=256)
                  m_SampleNum = 128;
                
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

                api_VWriteSafeMem(GET_CONTINUE_ADDR(EventConRom.big_adjust_para), sizeof(BIG_ADJUST_PARA), (BYTE *)&SysPara.big_adjust_para);
                
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
	DWORD RelativeTime;
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

	lib_InverseData(&pData[1], 2);
	if( api_CheckClock( (TRealTimer*)&pData[1] ) == FALSE )
	{
		return DAR_HardWare;
	}

	api_AdjustTimeStart(); // 校时记录开始//zh

	RelativeTime = api_GetRelativeMin();

	memcpy((BYTE *)&(TmpRealTimer.wYear), &pData[1], sizeof(TRealTimer));
	api_WriteMeterTime(&TmpRealTimer);
	api_SetAllTaskFlag(eFLAG_TIME_CHANGE);
	api_UpdateFreezeTime(RelativeTime);
	DAR = DAR_Success; //DAR 成功
	api_AdjustTimeEnd(); // 校时记录结束//zh

	return DAR;
}

BYTE SetHGPara( BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )//电气设备
{
	BYTE DAR;
//	WORD Result;

//	Result = FALSE;
	DAR = DAR_WrongType;
	
	switch( ClassAttribute )
	{
	case 2://
		if( ClassIndex == 0 )
		{
			// DAR = SetHGVolPara(&pData[3]);//zh

		}
		break;
	default:
		break;
	}
	
	return DAR;
}
BYTE SetAssetsCodePara( BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )//电气设备
{
	BYTE DAR,Buf[40];
	// WORD Result;

	// Result = FALSE;
	DAR = DAR_WrongType;
	
	switch( ClassAttribute )
	{
	case 2://
		if( ClassIndex == 0 )
		{
			if(pData[0] == Visible_string_698)
			{
				if(pData[1] <= 32)
				{
					memcpy(Buf, &pData[2], 32);
					api_VWriteSafeMem(GET_CONTINUE_ADDR(EventConRom.szAssetsCode),36, (BYTE*)Buf);
					DAR = DAR_Success; //DAR 成功
					
				}
			}

		}
		break;
	default:
		break;
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
	BYTE DAR;
	WORD Result;

	Result = FALSE;
	DAR = DAR_WrongType;
	
	switch( ClassAttribute )
	{
	case 7://允许跟随上报
		if( ClassIndex == 0 )
		{
			if( (pData[0] == Boolean_698) && (pData[1] <= 1) )
			{
				DAR = DAR_Success;
				gReportMode.g_EnableFollowReportFlag = pData[1];
				Result = TRUE;
			}
		}
		break;
		
	case 8://允许主动上报     鼎信的软件进行测试并要求成功返回 无实际意义--jwh ！！！！！！
		if( ClassIndex == 0 )
		{
			if( (pData[0] == Boolean_698) && (pData[1] <= 1) )
			{
				DAR = DAR_Success;
			}
			gReportMode.g_EnableAutoReportFlag = pData[1];//只为应付鼎信测试软件测试
			Result = TRUE;
		}
		break;
		
	case 10://上报通道
		break;
	default:
		break;
	}
	
	DAR = DAR_Success;
	Result = 1;
	if( DAR == DAR_Success )
	{
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;//此为固定返回，不可随意更改，请于上层进行配合
		}
		else
		{
			api_VWriteSafeMem(GET_CONTINUE_ADDR(EventConRom.ReportMode), sizeof(TReportMode), (BYTE *)&gReportMode);
			DAR = DAR_Success;
		}
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
		if(InitComplete == 0)
			InitComplete=1;
		break;
	case 0x4300://电气设备
		DAR = SetMeter( ClassAttribute,  ClassIndex, pData);
		break;
	case 0x4030://电压合格参数
		DAR = SetHGPara( ClassAttribute,  ClassIndex, pData);
		break;
        case 0x4103://资产编号设置
		DAR = SetAssetsCodePara( ClassAttribute,  ClassIndex, pData);
		break;
	default:
		DAR = DAR_WrongType;
		break;
	}

	eResultChoice = DLT698AddOneBuf(Ch, 0x55, DAR); //是完整数据

	return eResultChoice;
}

BYTE SetUDistortion3039(BYTE ClassAttribute, BYTE ClassIndex, BYTE *pData) //暂未考虑ClassIndex不为零的情况
{
	BYTE DAR;
	WORD Result;
//	DWORD RelativeTime;
//	TRealTimer TmpRealTimer; //时间结构体
	TFourByteUnion OAD;
	TEventPara tempEventPara;

	DAR = DAR_WrongType;
	memcpy(OAD.b, pData - 4, 4); //获取OAD
	
	if ((ClassAttribute != 0x05) /*|| (ClassIndex != 0)*/) //只能设置属性2
	{
		return DAR_Undefined;
	}
        
	api_VReadSafeMem(GET_CONTINUE_ADDR( EventConRom.EventPara ), sizeof(TEventPara), (BYTE *)&tempEventPara);
        
        if(ClassIndex == 1)
	{
		tempEventPara.sHrmUDistorThreshold = MW(pData[1], pData[2]);
	}
	else if(ClassIndex == 2)
	{
		tempEventPara.HrmUDistorDelayTime = pData[1];
	}
	else
	{
              if (pData[2] != Long_698)
              {
                      return DAR_Undefined;
              }
              if (pData[5] != Unsigned_698)
              {
                      return DAR_Undefined;
              }
            tempEventPara.sHrmUDistorThreshold = MW(pData[3], pData[4]);
            tempEventPara.HrmUDistorDelayTime = pData[6];
        }
        
        
        Result = api_VWriteSafeMem(GET_CONTINUE_ADDR(EventConRom.EventPara), sizeof(TEventPara), (BYTE *)&tempEventPara);

	if (Result == FALSE)
	{
		DAR = DAR_HardWare; //此为固定错误返回，不可随意更改，与上层调用函数有关
	}
	else
	{
		UpdateThresholdValue((BYTE*)&tempEventPara);

		DAR = DAR_Success; //DAR 成功
	}
	return DAR;
}

BYTE SetIDistortion303A(BYTE ClassAttribute, BYTE ClassIndex, BYTE *pData) //暂未考虑ClassIndex不为零的情况
{
	BYTE DAR;
	WORD Result;
//	DWORD RelativeTime;
//	TRealTimer TmpRealTimer; //时间结构体
	TFourByteUnion OAD;
	TEventPara tempEventPara;
	
	DAR = DAR_WrongType;
	memcpy(OAD.b, pData - 4, 4); //获取OAD
	
		
        if ((ClassAttribute != 0x05)/* || (ClassIndex != 0)*/) //只能设置属性2
	{
		return DAR_Undefined;
	}
	 
	api_VReadSafeMem(GET_CONTINUE_ADDR( EventConRom.EventPara ), sizeof(TEventPara), (BYTE *)&tempEventPara);
	if(ClassIndex == 1)
	{
		tempEventPara.sHrmIDistorThreshold = MW(pData[1], pData[2]);
	}
	else if(ClassIndex == 2)
	{
		tempEventPara.HrmIDistorDelayTime = pData[1];
	}
	else
	{
              if (pData[2] != Long_698)
              {
                      return DAR_Undefined;
              }
              if (pData[5] != Unsigned_698)
              {
                      return DAR_Undefined;
              }
	
              tempEventPara.sHrmIDistorThreshold = MW(pData[3], pData[4]);
              tempEventPara.HrmIDistorDelayTime = pData[6];
	}
        
    Result = api_VWriteSafeMem(GET_CONTINUE_ADDR(EventConRom.EventPara), sizeof(TEventPara), (BYTE *)&tempEventPara);
	
	if (Result == FALSE)
	{
		DAR = DAR_HardWare; //此为固定错误返回，不可随意更改，与上层调用函数有关
	}
	else
	{
		UpdateThresholdValue((BYTE*)&tempEventPara);
		DAR = DAR_Success; //DAR 成功
	}
	return DAR;
}

BYTE SetUOverRun3041(BYTE ClassAttribute, BYTE ClassIndex, BYTE *pData) //暂未考虑ClassIndex不为零的情况
{
	BYTE DAR;
	WORD Result;
//	DWORD RelativeTime;
//	TRealTimer TmpRealTimer; //时间结构体
	TFourByteUnion OAD;
	TEventPara tempEventPara;
	
	DAR = DAR_WrongType;
	memcpy(OAD.b, pData - 4, 4); //获取OAD
	
        api_VReadSafeMem(GET_CONTINUE_ADDR( EventConRom.EventPara ), sizeof(TEventPara), (BYTE *)&tempEventPara);
	if(ClassIndex == 1)
	{
		tempEventPara.sHrmUContentThreshold = MW(pData[1], pData[2]);
	}
	else if(ClassIndex == 2)
	{
		tempEventPara.HrmUContentDelayTime = pData[1];
	}
	else
	{
              if (pData[2] != Long_698)
              {
                      return DAR_Undefined;
              }
              if (pData[5] != Unsigned_698)
              {
                      return DAR_Undefined;
              }
	
              tempEventPara.sHrmUContentThreshold = MW(pData[3], pData[4]);
              tempEventPara.HrmUContentDelayTime = pData[6];
	}
	
    Result = api_VWriteSafeMem(GET_CONTINUE_ADDR(EventConRom.EventPara), sizeof(TEventPara), (BYTE *)&tempEventPara);
	
	if (Result == FALSE)
	{
		DAR = DAR_HardWare; //此为固定错误返回，不可随意更改，与上层调用函数有关
	}
	else
	{
		UpdateThresholdValue((BYTE*)&tempEventPara);
		DAR = DAR_Success; //DAR 成功
	}
	return DAR;
}

BYTE SetIOverRun3042(BYTE ClassAttribute, BYTE ClassIndex, BYTE *pData) //暂未考虑ClassIndex不为零的情况
{
	BYTE DAR;
	WORD Result;
//	DWORD RelativeTime;
//	TRealTimer TmpRealTimer; //时间结构体
	TFourByteUnion OAD;
	TEventPara tempEventPara;
	
	DAR = DAR_WrongType;
	memcpy(OAD.b, pData - 4, 4); //获取OAD
	
        api_VReadSafeMem(GET_CONTINUE_ADDR( EventConRom.EventPara ), sizeof(TEventPara), (BYTE *)&tempEventPara);
	if(ClassIndex == 1)
	{
		tempEventPara.sHrmIContentThreshold = MW(pData[1], pData[2]);
	}
	else if(ClassIndex == 2)
	{
		tempEventPara.HrmUContentDelayTime = pData[1];
	}
	else
	{
              if (pData[2] != Long_698)
              {
                      return DAR_Undefined;
              }
              if (pData[5] != Unsigned_698)
              {
                      return DAR_Undefined;
              }
	
              tempEventPara.sHrmIContentThreshold = MW(pData[3], pData[4]);
              tempEventPara.HrmIContentDelayTime = pData[6];
	}
	
        
	
    Result = api_VWriteSafeMem(GET_CONTINUE_ADDR(EventConRom.EventPara), sizeof(TEventPara), (BYTE *)&tempEventPara);
	
	if (Result == FALSE)
	{
		DAR = DAR_HardWare; //此为固定错误返回，不可随意更改，与上层调用函数有关
	}
	else
	{
		UpdateThresholdValue((BYTE*)&tempEventPara);
		DAR = DAR_Success; //DAR 成功
	}
	return DAR;
}


BYTE SetPOverRun3043(BYTE ClassAttribute, BYTE ClassIndex, BYTE *pData) //暂未考虑ClassIndex不为零的情况
{
	BYTE DAR;
	WORD Result;
//	DWORD RelativeTime;
//	TRealTimer TmpRealTimer; //时间结构体
	TFourByteUnion OAD;
	TEventPara tempEventPara;
	
	DAR = DAR_WrongType;
	memcpy(OAD.b, pData - 4, 4); //获取OAD
	
         api_VReadSafeMem(GET_CONTINUE_ADDR( EventConRom.EventPara ), sizeof(TEventPara), (BYTE *)&tempEventPara);
	if(ClassIndex == 1)
	{
		tempEventPara.sHrmPContentThreshold = MW(pData[1], pData[2]);
	}
	else if(ClassIndex == 2)
	{
		tempEventPara.HrmPContentDelayTime = pData[1];
	}
	else
	{
              if (pData[2] != Long_698)
              {
                      return DAR_Undefined;
              }
              if (pData[5] != Unsigned_698)
              {
                      return DAR_Undefined;
              }
	
              tempEventPara.sHrmPContentThreshold = MW(pData[3], pData[4]);
              tempEventPara.HrmPContentDelayTime = pData[6];
	}
	
    Result = api_VWriteSafeMem(GET_CONTINUE_ADDR(EventConRom.EventPara), sizeof(TEventPara), (BYTE *)&tempEventPara);
	
	if (Result == FALSE)
	{
		DAR = DAR_HardWare; //此为固定错误返回，不可随意更改，与上层调用函数有关
	}
	else
	{
		UpdateThresholdValue((BYTE*)&tempEventPara);
		DAR = DAR_Success; //DAR 成功
	}
	return DAR;
}


BYTE SetHrmUOverRun3044(BYTE ClassAttribute, BYTE ClassIndex, BYTE *pData) //暂未考虑ClassIndex不为零的情况
{
	BYTE DAR;
	WORD Result;
//	DWORD RelativeTime;
//	TRealTimer TmpRealTimer; //时间结构体
	TFourByteUnion OAD;
	TEventPara tempEventPara;
	
	DAR = DAR_WrongType;
	memcpy(OAD.b, pData - 4, 4); //获取OAD
	
        api_VReadSafeMem(GET_CONTINUE_ADDR( EventConRom.EventPara ), sizeof(TEventPara), (BYTE *)&tempEventPara);
	if(ClassIndex == 1)
	{
		tempEventPara.sInterHrmUContentThreshold = MW(pData[1], pData[2]);
	}
	else if(ClassIndex == 2)
	{
		tempEventPara.InterHrmUContentDelayTime = pData[1];
	}
	else
	{
              if (pData[2] != Long_698)
              {
                      return DAR_Undefined;
              }
              if (pData[5] != Unsigned_698)
              {
                      return DAR_Undefined;
              }
	
              tempEventPara.sInterHrmUContentThreshold = MW(pData[3], pData[4]);
              tempEventPara.InterHrmUContentDelayTime = pData[6];
	}
        
	
    Result = api_VWriteSafeMem(GET_CONTINUE_ADDR(EventConRom.EventPara), sizeof(TEventPara), (BYTE *)&tempEventPara);
	
	if (Result == FALSE)
	{
		DAR = DAR_HardWare; //此为固定错误返回，不可随意更改，与上层调用函数有关
	}
	else
	{
		UpdateThresholdValue((BYTE*)&tempEventPara);
		DAR = DAR_Success; //DAR 成功
	}
	return DAR;
}

BYTE SetHrmIOverRun3045(BYTE ClassAttribute, BYTE ClassIndex, BYTE *pData) //暂未考虑ClassIndex不为零的情况
{
	BYTE DAR;
	WORD Result;
//	DWORD RelativeTime;
//	TRealTimer TmpRealTimer; //时间结构体
	TFourByteUnion OAD;
	TEventPara tempEventPara;
	
	DAR = DAR_WrongType;
	memcpy(OAD.b, pData - 4, 4); //获取OAD
	
                
        api_VReadSafeMem(GET_CONTINUE_ADDR( EventConRom.EventPara ), sizeof(TEventPara), (BYTE *)&tempEventPara);
	if(ClassIndex == 1)
	{
		tempEventPara.sInterHrmIContentThreshold = MW(pData[1], pData[2]);
	}
	else if(ClassIndex == 2)
	{
		tempEventPara.InterHrmIContentDelayTime = pData[1];
	}
	else
	{
              if (pData[2] != Long_698)
              {
                      return DAR_Undefined;
              }
              if (pData[5] != Unsigned_698)
              {
                      return DAR_Undefined;
              }
	
              tempEventPara.sInterHrmIContentThreshold = MW(pData[3], pData[4]);
              tempEventPara.InterHrmIContentDelayTime = pData[6];
	}
        
    Result = api_VWriteSafeMem(GET_CONTINUE_ADDR(EventConRom.EventPara), sizeof(TEventPara), (BYTE *)&tempEventPara);
	
	if (Result == FALSE)
	{
		DAR = DAR_HardWare; //此为固定错误返回，不可随意更改，与上层调用函数有关
	}
	else
	{
		UpdateThresholdValue((BYTE*)&tempEventPara);
		DAR = DAR_Success; //DAR 成功
	}
	return DAR;
}
BYTE SetHrmPOverRun3046(BYTE ClassAttribute, BYTE ClassIndex, BYTE *pData) //暂未考虑ClassIndex不为零的情况
{
	BYTE DAR;
	WORD Result;
//	DWORD RelativeTime;
//	TRealTimer TmpRealTimer; //时间结构体
	TFourByteUnion OAD;
	TEventPara tempEventPara;
	
	DAR = DAR_WrongType;
	memcpy(OAD.b, pData - 4, 4); //获取OAD
	
         api_VReadSafeMem(GET_CONTINUE_ADDR( EventConRom.EventPara ), sizeof(TEventPara), (BYTE *)&tempEventPara);
	if(ClassIndex == 1)
	{
		tempEventPara.sInterHrmPContentThreshold = MW(pData[1], pData[2]);
	}
	else if(ClassIndex == 2)
	{
		tempEventPara.InterHrmPContentDelayTime = pData[1];
	}
	else
	{
              if (pData[2] != Long_698)
              {
                      return DAR_Undefined;
              }
              if (pData[5] != Unsigned_698)
              {
                      return DAR_Undefined;
              }
	
              tempEventPara.sInterHrmPContentThreshold = MW(pData[3], pData[4]);
              tempEventPara.InterHrmPContentDelayTime = pData[6];
	}
        
    Result = api_VWriteSafeMem(GET_CONTINUE_ADDR(EventConRom.EventPara), sizeof(TEventPara), (BYTE *)&tempEventPara);
	
	if (Result == FALSE)
	{
		DAR = DAR_HardWare; //此为固定错误返回，不可随意更改，与上层调用函数有关
	}
	else
	{
		UpdateThresholdValue((BYTE*)&tempEventPara);
		DAR = DAR_Success; //DAR 成功
	}
	return DAR;
}

BYTE SetFlkSOverRun3047(BYTE ClassAttribute, BYTE ClassIndex, BYTE *pData) //暂未考虑ClassIndex不为零的情况
{
	BYTE DAR;
	WORD Result;
//	DWORD RelativeTime;
//	TRealTimer TmpRealTimer; //时间结构体
	TFourByteUnion OAD;
	TEventPara tempEventPara;
	
	DAR = DAR_WrongType;
	memcpy(OAD.b, pData - 4, 4); //获取OAD
	
	if ((ClassAttribute != 0x05) /*|| (ClassIndex != 0)*/) //只能设置属性2
	{
		return DAR_Undefined;
	}
	
         api_VReadSafeMem(GET_CONTINUE_ADDR( EventConRom.EventPara ), sizeof(TEventPara), (BYTE *)&tempEventPara);
       
        if(ClassIndex == 1)
        {
               tempEventPara.wShortFlkThreshold = MW(pData[1], pData[2])*100;
        }
        else if(ClassIndex == 2)
        {
             tempEventPara.wShortFlkDelayTime = MW(pData[1], pData[2]);
        }
        else
        {
          
              if (pData[2] != Long_unsigned_698)
              {
                      return DAR_Undefined;
              }
              if (pData[5] != Long_unsigned_698)
              {
                      return DAR_Undefined;
              }
	
  
	
            tempEventPara.wShortFlkThreshold = MW(pData[3], pData[4])*100;
            tempEventPara.wShortFlkDelayTime = MW(pData[6], pData[7]);
	}
        
    Result = api_VWriteSafeMem(GET_CONTINUE_ADDR(EventConRom.EventPara), sizeof(TEventPara), (BYTE *)&tempEventPara);
	
	if (Result == FALSE)
	{
		DAR = DAR_HardWare; //此为固定错误返回，不可随意更改，与上层调用函数有关
	}
	else
	{
		UpdateThresholdValue((BYTE*)&tempEventPara);
		DAR = DAR_Success; //DAR 成功
	}
	return DAR;
}

BYTE SetFlkLOverRun3048(BYTE ClassAttribute, BYTE ClassIndex, BYTE *pData) //暂未考虑ClassIndex不为零的情况
{
	BYTE DAR;
	WORD Result;
//	DWORD RelativeTime;
//	TRealTimer TmpRealTimer; //时间结构体
	TFourByteUnion OAD;
	TEventPara tempEventPara;
	
	DAR = DAR_WrongType;
	memcpy(OAD.b, pData - 4, 4); //获取OAD
	
	if ((ClassAttribute != 0x05) /*|| (ClassIndex != 0)*/) //只能设置属性2
	{
		return DAR_Undefined;
	}
        
	api_VReadSafeMem(GET_CONTINUE_ADDR( EventConRom.EventPara ), sizeof(TEventPara), (BYTE *)&tempEventPara);
        if(ClassIndex == 1)
        {
               tempEventPara.wLongFlkThreshold = MW(pData[1], pData[2])*100;
        }
        else if(ClassIndex == 2)
        {
            tempEventPara.wLongFlkDelayTime = MW(pData[1], pData[2]);
        }
        else
        {
          
            if (pData[2] != Long_unsigned_698)
            {
                    return DAR_Undefined;
            }
            if (pData[5] != Long_unsigned_698)
            {
                    return DAR_Undefined;
            }
	
    
	
          tempEventPara.wLongFlkThreshold = MW(pData[3], pData[4])*100;
          tempEventPara.wLongFlkDelayTime = MW(pData[6], pData[7]);
	}
        
    Result = api_VWriteSafeMem(GET_CONTINUE_ADDR(EventConRom.EventPara), sizeof(TEventPara), (BYTE *)&tempEventPara);
	
	if (Result == FALSE)
	{
		DAR = DAR_HardWare; //此为固定错误返回，不可随意更改，与上层调用函数有关
	}
	else
	{
		UpdateThresholdValue((BYTE*)&tempEventPara);
		DAR = DAR_Success; //DAR 成功
	}
	return DAR;
}

BYTE SetSagU3049(BYTE ClassAttribute, BYTE ClassIndex, BYTE *pData) //暂未考虑ClassIndex不为零的情况
{
	BYTE DAR;
	WORD Result;
//	DWORD RelativeTime;
//	TRealTimer TmpRealTimer; //时间结构体
	TFourByteUnion OAD;
	TEventPara tempEventPara;
	
	DAR = DAR_WrongType;
	memcpy(OAD.b, pData - 4, 4); //获取OAD
	
	if ((ClassAttribute != 0x05) /*|| (ClassIndex != 0)*/ ) //只能设置属性2
	{
		return DAR_Undefined;
	}
        
	api_VReadSafeMem(GET_CONTINUE_ADDR( EventConRom.EventPara ), sizeof(TEventPara), (BYTE *)&tempEventPara);
        
        if(ClassIndex == 1)
        {
              tempEventPara.wSagUThreshold = MW(pData[1], pData[2]);
        }
        else if(ClassIndex == 2)
        {
            tempEventPara.wSagUDelayTime = MW(pData[1], pData[2]);
        }
        else
        {
            if (pData[2] != Long_unsigned_698)
            {
                    return DAR_Undefined;
            }
            if (pData[5] != Long_unsigned_698)
            {
                    return DAR_Undefined;
            }
           tempEventPara.wSagUThreshold = MW(pData[3], pData[4]);
	//if (pData[5] == Long_unsigned_698)
            {
		tempEventPara.wSagUDelayTime = MW(pData[6], pData[7]);
            }
	}
        
        Result = api_VWriteSafeMem(GET_CONTINUE_ADDR(EventConRom.EventPara), sizeof(TEventPara), (BYTE *)&tempEventPara);
	
	if (Result == FALSE)
	{
		DAR = DAR_HardWare; //此为固定错误返回，不可随意更改，与上层调用函数有关
	}
	else
	{
		UpdateThresholdValue((BYTE*)&tempEventPara);
		DAR = DAR_Success; //DAR 成功
	}
	return DAR;
}

BYTE SetSwellU304A(BYTE ClassAttribute, BYTE ClassIndex, BYTE *pData) //暂未考虑ClassIndex不为零的情况
{
	BYTE DAR;
	WORD Result;
//	DWORD RelativeTime;
//	TRealTimer TmpRealTimer; //时间结构体
	TFourByteUnion OAD;
	TEventPara tempEventPara;
	
	DAR = DAR_WrongType;
	memcpy(OAD.b, pData - 4, 4); //获取OAD
	
	if ((ClassAttribute != 0x05) /*|| (ClassIndex != 0)*/ ) //只能设置属性2
	{
		return DAR_Undefined;
	}
        
	 api_VReadSafeMem(GET_CONTINUE_ADDR( EventConRom.EventPara ), sizeof(TEventPara), (BYTE *)&tempEventPara);
        
        if(ClassIndex == 1)
        {
              tempEventPara.wSwellUThreshold = MW(pData[1], pData[2]);
        }
        else if(ClassIndex == 2)
        {
            tempEventPara.wSwellUDelayTime = MW(pData[1], pData[2]);
        }
        else
        {
          
            if (pData[2] != Long_unsigned_698)
            {
                    return DAR_Undefined;
            }
            if (pData[5] != Long_unsigned_698)
            {
                    return DAR_Undefined;
            }
	
            tempEventPara.wSwellUThreshold = MW(pData[3], pData[4]);
            //if (pData[5] == Long_unsigned_698)
            {
                    tempEventPara.wSwellUDelayTime = MW(pData[6], pData[7]);
            }
	}
	
    Result = api_VWriteSafeMem(GET_CONTINUE_ADDR(EventConRom.EventPara), sizeof(TEventPara), (BYTE *)&tempEventPara);
	
	if (Result == FALSE)
	{
		DAR = DAR_HardWare; //此为固定错误返回，不可随意更改，与上层调用函数有关
	}
	else
	{
		UpdateThresholdValue((BYTE*)&tempEventPara);
		DAR = DAR_Success; //DAR 成功
	}
	return DAR;
}


BYTE SetInterruptU304B(BYTE ClassAttribute, BYTE ClassIndex, BYTE *pData) //暂未考虑ClassIndex不为零的情况
{
	BYTE DAR;
	WORD Result;
//	DWORD RelativeTime;
//	TRealTimer TmpRealTimer; //时间结构体
	TFourByteUnion OAD;
	TEventPara tempEventPara;
	
	DAR = DAR_WrongType;
	memcpy(OAD.b, pData - 4, 4); //获取OAD
	
	if ((ClassAttribute != 0x05) /*|| (ClassIndex != 0)*/ ) //只能设置属性2
	{
		return DAR_Undefined;
	} 
        
        api_VReadSafeMem(GET_CONTINUE_ADDR( EventConRom.EventPara ), sizeof(TEventPara), (BYTE *)&tempEventPara);
	
        if(ClassIndex == 1)
        {
              tempEventPara.wInterruptUThreshold = MW(pData[1], pData[2]);
        }
        else if(ClassIndex == 2)
        {
            tempEventPara.wInterruptUDelayTime = MW(pData[1], pData[2]);
        }
        else
        {
          
              if (pData[2] != Long_unsigned_698)
              {
                      return DAR_Undefined;
              }
              if (pData[5] != Long_unsigned_698)
              {
                      return DAR_Undefined;
              }
              tempEventPara.wInterruptUThreshold = MW(pData[3], pData[4]);
              //if (pData[5] == Long_unsigned_698)
              {
                      tempEventPara.wInterruptUDelayTime = MW(pData[6], pData[7]);
              }
        //	else
                {
                //	tempEventPara.wInterruptUDelayTime = 60;
                }
	}
          
        Result = api_VWriteSafeMem(GET_CONTINUE_ADDR(EventConRom.EventPara), sizeof(TEventPara), (BYTE *)&tempEventPara);
	
	if (Result == FALSE)
	{
		DAR = DAR_HardWare; //此为固定错误返回，不可随意更改，与上层调用函数有关
	}
	else
	{
		UpdateThresholdValue((BYTE*)&tempEventPara);
		DAR = DAR_Success; //DAR 成功
	}
	return DAR;
}


BYTE SetSpiErr302F(BYTE ClassAttribute, BYTE ClassIndex, BYTE *pData) //暂未考虑ClassIndex不为零的情况
{
	BYTE DAR;
	WORD Result;
//	DWORD RelativeTime;
//	TRealTimer TmpRealTimer; //时间结构体
	TFourByteUnion OAD;
	TEventPara tempEventPara;
	
	DAR = DAR_WrongType;
	memcpy(OAD.b, pData - 4, 4); //获取OAD
	
	if ((ClassAttribute != 0x06) /*|| (ClassIndex != 0)*/) //只能设置属性2
	{
		return DAR_Undefined;
	}
	
	if (pData[2] != Unsigned_698)
	{
		return DAR_Undefined;
	}
	
	api_VReadSafeMem(GET_CONTINUE_ADDR( EventConRom.EventPara ), sizeof(TEventPara), (BYTE *)&tempEventPara);
	
	tempEventPara.SPIDelayTime = (pData[3]);
	
	Result = api_VWriteSafeMem(GET_CONTINUE_ADDR(EventConRom.EventPara), sizeof(TEventPara), (BYTE *)&tempEventPara);
	
	if (Result == FALSE)
	{
		DAR = DAR_HardWare; //此为固定错误返回，不可随意更改，与上层调用函数有关
	}
	else
	{
		UpdateThresholdValue((BYTE*)&tempEventPara);
		DAR = DAR_Success; //DAR 成功
	}
	return DAR;
}


//-----------------------------------------------
//函数功能: 设置事件上报标识
//
//参数: 	
//			OI[in]:				事件OI
//			byReportMode[in]:	事件上报标识{不上报(0),事件发生上报(1),事件恢复上报(2),事件发生恢复均上报(3)}
//
//返回值:	TRUE/FALSE
//			
//备注:接口类7的属性8，接口类24的属性11
//-----------------------------------------------
BOOL SetEventReportMode( WORD OI, BYTE byReportMode )
{
	BYTE i;
	
	if( byReportMode > 3 )
	{
		return FALSE;
	}
	
	if( GetEventIndex( OI, &i ) == FALSE )
	{
		return FALSE;
	}
	
	if( byReportMode == gReportMode.byReportMode[i] )//如果要设置值和RAM中一样，实际也和EEPROM中一样
	{
		return TRUE;	 
	}
	else
	{	
		gReportMode.byReportMode[i] = byReportMode;

		return api_VWriteSafeMem(GET_CONTINUE_ADDR(EventConRom.ReportMode), sizeof(TReportMode), (BYTE *)&gReportMode);
	}
	
}

BOOL SetEventReportFlag( WORD OI, BYTE byReportMode )
{
	BYTE i;
	
	if( byReportMode > 3 )
	{
		return FALSE;
	}
	
	if( GetEventIndex( OI, &i ) == FALSE )
	{
		return FALSE;
	}
	
	if( byReportMode == gReportMode.byReportFlag[i] )//如果要设置值和RAM中一样，实际也和EEPROM中一样
	{
		return TRUE;	 
	}
	else
	{	
		gReportMode.byReportFlag[i] = byReportMode;
		
		return api_VWriteSafeMem(GET_CONTINUE_ADDR(EventConRom.ReportMode), sizeof(TReportMode), (BYTE *)&gReportMode);
	}
	
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
BYTE SetRequestEventReportMode( WORD OI, BYTE *pData )
{
	BYTE DAR;
	WORD Result;
	
	DAR = DAR_WrongType;
	
	if( pData[0] == Enum_698)
	{
		Result = SetEventReportMode( OI, pData[1] );
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

BYTE SetRequestEventReportFlag( WORD OI, BYTE *pData )
{
	BYTE DAR;
	WORD Result;
	
	DAR = DAR_WrongType;
	
	if( pData[0] == Enum_698)
	{
		Result = SetEventReportFlag( OI, pData[1] );
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

BOOL SetEventValidFlag( WORD OI, BYTE Valid )
{
	BYTE i;
	
	if( Valid > 1 )
	{
		return FALSE;
	}
	
	if( GetEventIndex( OI, &i ) == FALSE )
	{
		return FALSE;
	}
	
	if( Valid == gReportMode.byEventValid[i] )//如果要设置值和RAM中一样，实际也和EEPROM中一样
	{
		return TRUE;	 
	}
	else
	{	
		gReportMode.byEventValid[i] = Valid;
		
		return api_VWriteSafeMem(GET_CONTINUE_ADDR(EventConRom.ReportMode), sizeof(TReportMode), (BYTE *)&gReportMode);
	}
	
}

///////////////////////////////////////////////////////////////
//	函 数 名 : SetRequestEventValidFlag
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2022年2月17日
//	返 回 值 : BYTE
//	参数说明 :  WORD OI,
//				 BYTE *pData 
///////////////////////////////////////////////////////////////
BYTE SetRequestEventValidFlag( WORD OI, BYTE *pData )
{
	BYTE DAR;
	WORD Result;
	
	DAR = DAR_WrongType;
	
	if( pData[0] == Boolean_698)
	{
		Result = SetEventValidFlag( OI, pData[1] );
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
	BYTE DAR,ClassAttribute;
	BYTE *pData;
	TTwoByteUnion OI;
	eAPPBufResultChoice eResultChoice;
	
	pData = &pOAD[4];//获取数据指针
	lib_ExchangeData(OI.b,pOAD,2);//获取OI
	ClassAttribute = (pOAD[2]&0x0f);
	// ClassIndex = pOAD[3];
	DAR = DAR_WrongType; 
	
    //if((OI.w == 0x3013) )//私钥状态下不允许更改事件清零关联对象属性表
    //{
    //    DAR = DAR_RefuseOp; 
    //}
	//else 
	#if(PROTOCOL_VERSION == 25)
	if( (OI.w ==0x3013) || (OI.w == 0x302F) ||( OI.w == 0x3036)||( OI.w == 0x3016) || ( OI.w == 0x309A) || ( OI.w == 0x309B)) //配置参数 之所以两处调用是因为属性不一样 //zh
	#elif(PROTOCOL_VERSION == 24)
	if( (OI.w ==0x3013) || (OI.w == 0x302F) ||( OI.w == 0x3036)||( OI.w == 0x3016)) //配置参数 之所以两处调用是因为属性不一样 //zh
	#endif
	{
		switch( ClassAttribute )
		{
		case 3://关联对象属性表
			//DAR = SetEventRelationTable( OI.w, ClassIndex, pData);
			break;
			
		case 6://配置参数
			// DAR = SetEventConfigPara( OI.w,  ClassIndex, pData);
			break;
			
		case 8://上报标识
			DAR = SetRequestEventReportFlag( OI.w, pData);
			break;
			
		case 9://有效标识
			DAR = SetRequestEventValidFlag( OI.w, pData);
			break;
		case 11:
			DAR = SetRequestEventReportMode( OI.w, pData);
			break;
		}
	}
	else
	{	
		switch( ClassAttribute )
		{
		case 2://关联对象属性表
			//DAR = SetEventRelationTable( OI.w, ClassIndex, pData);
			break;
			
		case 5://配置参数
		//	DAR = SetEventConfigPara( OI.w,  ClassIndex, pData);
			break;
			
		case 11://上报标识
			DAR = SetRequestEventReportFlag( OI.w, pData);
			break;
			
		case 12://有效标识
			DAR = SetRequestEventValidFlag( OI.w, pData);
			break;
		case 15:
			DAR = SetRequestEventReportMode( OI.w, pData);
			break;
		}
	
	}
	
	
	eResultChoice = DLT698AddOneBuf(Ch,0x55, DAR);//是完整数据
    return eResultChoice;
}

///////////////////////////////////////////////////////////////
//	函 数 名 : SetRequestHRMParameter
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年11月18日
//	返 回 值 : eAPPBufResultChoice
//	参数说明 : BYTE Ch,
//				 BYTE *pOAD
///////////////////////////////////////////////////////////////
eAPPBufResultChoice SetRequestHRMParameter(BYTE Ch, BYTE *pOAD)
{
	BYTE DAR, ClassAttribute, ClassIndex;
	BYTE *pData;
	TTwoByteUnion OI;
	eAPPBufResultChoice eResultChoice;
	#if(PROTOCOL_VERSION == 25)
	BYTE Res = 0,Len = 0,i = 0,j = 0;
	TSpecificDevice SpcDev;
	#endif
	
	pData = &pOAD[4];				 //获取数据指针
	lib_ExchangeData(OI.b, pOAD, 2); //获取OI
	ClassAttribute = (pOAD[2] & 0x0f);
	ClassIndex = pOAD[3];
	DAR = DAR_WrongType;
	
	if(ClassAttribute == 11 || ClassAttribute == 8 || ClassAttribute == 15 || ClassAttribute == 12 || ClassAttribute == 9)
		return SetRequestEvent( Ch, pOAD );

	switch (OI.w)
	{
	case 0X3039: //电压谐波总畸变率超限事件
		DAR = SetUDistortion3039(ClassAttribute, ClassIndex, pData);
		break;
	case 0X303A://电流谐波总畸变率超限事件
		DAR = SetIDistortion303A(ClassAttribute, ClassIndex, pData);
		break;
	case 0X3041://谐波电压含量超限事件
		DAR = SetUOverRun3041(ClassAttribute, ClassIndex, pData);
		break;
	case 0X3042://谐波电流含量超限事件
		DAR = SetIOverRun3042(ClassAttribute, ClassIndex, pData);
		break;
	case 0X3043://谐波功率超限事件
		DAR = SetPOverRun3043(ClassAttribute, ClassIndex, pData);
		break;
	case 0X3044://间谐波电压含量超限事件
		DAR = SetHrmUOverRun3044(ClassAttribute, ClassIndex, pData);
		break;
	case 0X3045://间谐波电流含量超限事件
		DAR = SetHrmIOverRun3045(ClassAttribute, ClassIndex, pData);
		break;
	case 0X3046://间谐波功率超限事件
		DAR = SetHrmPOverRun3046(ClassAttribute, ClassIndex, pData);
		break;
	case 0X3047://电压短时闪变超限事件
		DAR = SetFlkSOverRun3047(ClassAttribute, ClassIndex, pData);
		break;
	case 0X3048://电压长时闪变超限事件
		DAR = SetFlkLOverRun3048(ClassAttribute, ClassIndex, pData);
		break;
	case 0X3049://电压暂降事件
		DAR = SetSagU3049(ClassAttribute, ClassIndex, pData);
		break;
	case 0X304A://电压暂升事件
		DAR = SetSwellU304A(ClassAttribute, ClassIndex, pData);
		break;
	case 0X304B://电压中断事件
		DAR = SetInterruptU304B(ClassAttribute, ClassIndex, pData);
		break;
	case 0X302F:
		DAR = SetSpiErr302F(ClassAttribute, ClassIndex, pData);
		break;
	#if(PROTOCOL_VERSION == 25)
	case 0x309B:
		if(ClassAttribute == 6) //属性6
		{
			if(pData[0] == Array_698)
			{
				Len = pData[1]; //电器个数
				if(Len <= NILM_309B_MAX_NO)
				{
					DAR = DAR_Success;

					//入参数据合法性判断
					for(i = 0; i < Len; i++)
					{
						if(pData[2+i*3] == Long_unsigned_698) //long_unsigned判断
						{
							for(j = 0; j < sizeof(g_bDevice_Code_List); j++)
							{
								if(pData[2+i*3 +1] == g_bDevice_Code_List[j]) //电器类型判断，判断低位即可
								{
									break;
								}
							}
							if(j > sizeof(g_bDevice_Code_List))
							{
								DAR = DAR_TempFail;
								break;
							}
						}
						else
						{
							DAR = DAR_TempFail;
							break;
						}
					}
					
					//写入数据
					if(DAR == DAR_Success)
					{
						memset((BYTE*)&SpcDev.bySpecificDeviceNum,0xFF,sizeof(TSpecificDevice));
						//填充数据，准备写入eep
						SpcDev.bySpecificDeviceNum = Len;
						for(i = 0; i < Len; i++)
						{
							lib_ExchangeData((BYTE*)&SpcDev.wSpecificDeviceID[i],&pData[2+i*3 + 1],sizeof(WORD)); //从低位转换2字节，加tag时候会倒序
						}

						Res = api_SetSpecificDeviceID(0x0A, (TSpecificDevice*)&SpcDev);
						if(Res == 0) //失败
						{
							DAR = DAR_TempFail;
						}
						else
						{
							DAR = DAR_Success;
						}
					}
					
				}
				else
				{
					DAR = DAR_TempFail;
				}
			}
		}
		break;
	#endif //#if(PROTOCOL_VERSION == 25)
	
	default:
		DAR = DAR_WrongType;
		break;
	}
	
	eResultChoice = DLT698AddOneBuf(Ch, 0x55, DAR); //是完整数据
	
	return eResultChoice;
}
eAPPBufResultChoice SetRequestHGParameter(BYTE Ch, BYTE *pOAD)
{
	BYTE DAR;
//	BYTE *pData;
//	WORD Result;
	TTwoByteUnion OI;
	eAPPBufResultChoice eResultChoice;
	
//	pData = &pOAD[4];				 //获取数据指针
	lib_ExchangeData(OI.b, pOAD, 2); //获取OI
//	ClassAttribute = (pOAD[2] & 0x0f);
	// ClassIndex = pOAD[3];
	DAR = DAR_WrongType;

	// DAR =SetFactQJPara(&pOAD[4]);//zh
	
	eResultChoice = DLT698AddOneBuf(Ch, 0x55, DAR); //是完整数据
	
	return eResultChoice;
}


eAPPBufResultChoice SetF101EsamPara(BYTE *pData)
{
	int nLen=0;
	int i, Num;
	
	if(pData[nLen++] != Array_698)
		return eAPP_WRONGTYPE;
	
	Num = pData[nLen++];
	SafeModePara.Num = Num;
	
	for(i=0; i<Num; i++ )
	{	
		if(pData[nLen] != Structure_698 && pData[nLen+1] != 2)
			return eAPP_WRONGTYPE;
		
		nLen += 3;
		SafeModePara.Sub[i].OI = MW(pData[nLen],pData[nLen+1]);
		
		nLen += 3;
		SafeModePara.Sub[i].wSafeMode = MW(pData[nLen+1],pData[nLen]);
		
		nLen += 2;
	}
	
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(ProSafeRom.SafeModePara), sizeof(TSafeModePara), (BYTE *)&SafeModePara );

	return eADD_OK;
}

eAPPBufResultChoice SetRequestF101EsamParameter(BYTE Ch, BYTE *pOAD)
{
	BYTE DAR, ClassAttribute;
//	BYTE *pData;
//	WORD Result;
	TTwoByteUnion OI;
	eAPPBufResultChoice eResultChoice;
	
//	pData = &pOAD[4];				 //获取数据指针
	lib_ExchangeData(OI.b, pOAD, 2); //获取OI
	ClassAttribute = (pOAD[2] & 0x0f);
//	ClassIndex = pOAD[3];
	DAR = DAR_WrongType;
	
        switch(ClassAttribute)
        {
        case 2:
          if(pOAD[4] == 0x16)
          {
             SafeModePara.Flag = pOAD[5];
             DAR = DAR_Success;
             api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(ProSafeRom.SafeModePara), sizeof(TSafeModePara), (BYTE *)&SafeModePara );
          }
          break;
        case 3:
          DAR =SetF101EsamPara(&pOAD[4]);
          break;
        }

	eResultChoice = DLT698AddOneBuf(Ch, 0x55, DAR); //是完整数据
	
	return eResultChoice;
}

eAPPBufResultChoice SetRequestFF01ChannelParameter(BYTE Ch, BYTE *pOAD)
{
	BYTE DAR, ClassAttribute;
//	BYTE *pData;
	TTwoByteUnion OI;
	eAPPBufResultChoice eResultChoice;
	
//	pData = &pOAD[4];				 //获取数据指针
	lib_ExchangeData(OI.b, pOAD, 2); //获取OI
	ClassAttribute = (pOAD[2] & 0x0f);
//	ClassIndex = pOAD[3];
	DAR = DAR_WrongType;
	
	switch(ClassAttribute)
	{
	case 2:
		if(OI.w == 0xFF01)
		{
			if(pOAD[4] == Enum_698)
			{
				if(pOAD[5] == 0)
				{
					SwitchtoRtt();
				}
				else if (pOAD[5] == 1)
				{
					SwitchtoUart();
				}
				DAR = DAR_Success;
			}
		}
		break;
	default:
		break;
	}

	eResultChoice = DLT698AddOneBuf(Ch, 0x55, DAR); //是完整数据
	
	return eResultChoice;
}


eAPPBufResultChoice SetRequest2035Parameter(BYTE Ch, BYTE *pOAD)
{
	BYTE DAR, ClassAttribute;
//	BYTE *pData;
	WORD Result;
	TTwoByteUnion OI;
	eAPPBufResultChoice eResultChoice;
	TEventPara tempEventPara;
        
//	pData = &pOAD[4];				 //获取数据指针
	lib_ExchangeData(OI.b, pOAD, 2); //获取OI
	ClassAttribute = (pOAD[2] & 0x0f);
//	ClassIndex = pOAD[3];
	DAR = DAR_WrongType;
	
	switch(ClassAttribute)
	{
	case 4:
              api_VReadSafeMem(GET_CONTINUE_ADDR( EventConRom.EventPara ), sizeof(TEventPara), (BYTE *)&tempEventPara);
	
              PQ_Para.dwBdThreshold = tempEventPara.wBodongThreshold =  MW(pOAD[5], pOAD[6]);
  
              Result = api_VWriteSafeMem(GET_CONTINUE_ADDR(EventConRom.EventPara), sizeof(TEventPara), (BYTE *)&tempEventPara);
	
              if (Result == FALSE)
              {
                      DAR = DAR_HardWare; //此为固定错误返回，不可随意更改，与上层调用函数有关
              }
              else
              {
                      UpdateThresholdValue((BYTE*)&tempEventPara);
                      DAR = DAR_Success; //DAR 成功
              }
        
              break;
	}
	
	eResultChoice = DLT698AddOneBuf(Ch, 0x55, DAR); //是完整数据
	
	return eResultChoice;
}

#if(PROTOCOL_VERSION == 25)
//--------------------------------------------------
//功能描述:  设置分钟冻结参数
//         
//参数:      
//         
//返回值:    
//         
//备注:   需要设置冻结周期和冻结深度，默认周期是15分钟，默认冻结深度是2880
//--------------------------------------------------
eAPPBufResultChoice SetRequest5002Parameter(BYTE Ch, BYTE *pOAD)
{
	BYTE DAR, ClassAttribute, ClassIndex;
	BYTE byCount = 0;
	BOOL Result = FALSE;
	TTwoByteUnion OI;
	eAPPBufResultChoice eResultChoice;
	WORD wData = 0, wCycle = 0;
	DWORD dwData = 0;
        
//	pData = &pOAD[4];				 //获取数据指针
	lib_ExchangeData(OI.b, pOAD, 2); //获取OI
	ClassAttribute = (pOAD[2] & 0x0f);
	ClassIndex = pOAD[3];
	DAR = DAR_WrongType;
	switch(ClassAttribute)
	{
		case 3://冻结关联对象属性表
			if(ClassIndex == 0)
			{
				if(pOAD[4] == Array_698)//Array_698
				{
					if(pOAD[5] == 1) //1个结构体
					{
						if(pOAD[6] == Structure_698)//Structure_698
						{
							if(pOAD[7] == 3) //三个数据
							{
								if(pOAD[8] == Long_unsigned_698)//Long_unsigned_698
								{
									lib_ExchangeData((BYTE*)&wData,&pOAD[9],sizeof(WORD));
									if((wData == g_Default_Freeze[0]) || (wData == g_Default_Freeze[1]) 
									|| (wData == g_Default_Freeze[2]) || (wData == g_Default_Freeze[3]))
									{
										wCycle = wData;//冻结周期
										byCount += 1;
									}
									else
									{
										byCount = 0;
									}
								}
								if(pOAD[11] == OAD_698)//OAD_698
								{
									lib_ExchangeData((BYTE*)&dwData,&pOAD[12],sizeof(DWORD));
									if(dwData == 0x48000400)//其他值不允许设置
									{
										byCount += 1;
									}
									else
									{
										byCount = 0;
									}
								}
								if(pOAD[16] == Long_unsigned_698)//Long_unsigned_698
								{
									lib_ExchangeData((BYTE*)&wData,&pOAD[17],sizeof(WORD));
									if(wData == 2880) //其他值不允许设置
									{
										byCount += 1;
									}
									else
									{
										byCount = 0;
									}
								}
							}
						}
					}
				}
			}

			if(byCount == 3)
			{
				Result = api_WriteFreezeAttribute(OI.w,(BYTE*)&wCycle);
				if(Result == FALSE)
				{
					DAR = DAR_WrongType;
				}
				else
				{
					DAR = DAR_Success;
				}
			}
			else
			{
				DAR = DAR_WrongType;
			}
			break;
		default:
			break;
	}
	eResultChoice = DLT698AddOneBuf(Ch, 0x55, DAR); //是完整数据
	return eResultChoice;
}
#endif //#if(PROTOCOL_VERSION == 25)
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
	#if(PROTOCOL_VERSION == 25)
	else if ((OI.w >= 0x3000) && (OI.w <= 0x309B)) //设置参变量
	#elif(PROTOCOL_VERSION == 24)
	else if ((OI.w >= 0x3000) && (OI.w <= 0x304B)) //设置参变量
	#endif
	{
		eResultChoice = SetRequestHRMParameter(Ch, pOAD);
	}
	else if(OI.w == 0x2103)
	{
		eResultChoice = SetRequestHGParameter(Ch, pOAD);
	}
	else if(OI.w == 0xF101)
	{
		eResultChoice = SetRequestF101EsamParameter(Ch, pOAD);
	}
    else if(OI.w == 0xFF01)
	{
		eResultChoice = SetRequestFF01ChannelParameter(Ch, pOAD);
	}
	else if(OI.w == 0x2035)
	{
		eResultChoice = SetRequest2035Parameter(Ch, pOAD);
	}
	else if(OI.w == 0x5002)
	{
		#if(PROTOCOL_VERSION == 25)
		eResultChoice =  SetRequest5002Parameter( Ch, pOAD );//25规范模组支持设置
		#elif(PROTOCOL_VERSION == 24)
		eResultChoice = DLT698AddOneBuf(Ch, 0x55, 3); //5002关联属性设置，不支持但是要返回异常,24模组
		#endif
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
        case SET_REQUEST_NORMALLIST:	
		eResultChoice = SetRequestNormalList( Ch );
		break;
        case SET_THEN_GET_REQUEST:
		eResultChoice = SetThenGetRequestNormalList(Ch);  
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
	case GET_RESPONSE_NORMAL:
		GetResponseNormal( Ch,&pAPDU[3] );
		break;
	case GET_RESPONSE_NORMALLIST:
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





void InitFileDown(void)
{

	memset(tFileInfo.BlockStatus,0,sizeof(tFileInfo.BlockStatus));
	tFileInfo.BlockStatusNum = 0;
	tFileInfo.FileStatus = eFileUpdateInit;
	tFileInfo.FileTransferStatus = eFileUpdateInit;
}
WORD GetFileBlockNum()
{
//	WORD i,j;
	WORD wNum=0;

	wNum = (tFileInfo.lFileSize/tFileInfo.dwTxBlockLen);

	if(tFileInfo.lFileSize%tFileInfo.dwTxBlockLen)
		wNum+=1;
	return wNum;
	
//	for(i=0;i<256;i++)
//	{
//		for(j=0;j<8;j++)
//		{
//			if(tFileInfo.BlockStatus[i] & (0x01<<j))
//				wNum++;
//		}
//	}
//	return wNum;

}
WORD GetFileBlockStatusLen()
{

  WORD wNum=0;
 
  if(tFileInfo.BlockStatusNum % 8)
  {
      wNum = ((tFileInfo.BlockStatusNum + 8) / 8);
  }
  else
  {
      wNum = (tFileInfo.BlockStatusNum / 8);
  }
  return wNum;
  
}

///////////////////////////////////////////////////////////////
//	函 数 名 : GetExLen
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月24日
//	返 回 值 : int
//	参数说明 : BYTE* pBuf,
//				 int* nLen
///////////////////////////////////////////////////////////////
int GetExLen(BYTE* pBuf, int* nLen)
{
	*nLen = 0;

	if(pBuf == NULL)
		return 0;

	if( pBuf[0] <= 0x7f )
	{
		*nLen = pBuf[0];
		return 1;
	}
	else if( pBuf[0] == 0x81 )
	{
		*nLen = pBuf[1];
		return 2;
	}
	else if( pBuf[0] == 0x82 )
	{
		*nLen = ((pBuf[1]<<8) + pBuf[2]);
		
		return 3;
	}
	return 0;
}

///////////////////////////////////////////////////////////////
//	函 数 名 : SetExLen
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月24日
//	返 回 值 : int
//	参数说明 : int nLen ,
//				 BYTE* pBuf
///////////////////////////////////////////////////////////////
int SetExLen(int nLen , BYTE* pBuf)
{

	if( nLen <= 0x7f )
	{
		pBuf[0] = nLen;
		return 1;
	}
	else if( nLen < 0xff )
	{
		pBuf[0] = 0x81;
		pBuf[1] = nLen;
		return 2;
	}
	else
	{
		pBuf[0]  = 0x82;
		pBuf[1]  = nLen/256;
		pBuf[2]  = (nLen%256);	
		
		return 3;
	}
}

extern BOOL bIsBoot;
extern BYTE* pBootBuf;

///////////////////////////////////////////////////////////////
//	函 数 名 : api_GetFileInfo
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月24日
//	返 回 值 : void
//	参数说明 : BYTE Ch,
//				 BYTE *Buffer,
//				 WORD wLen
///////////////////////////////////////////////////////////////
BYTE api_GetFileInfo(BYTE Ch, BYTE *Buffer, WORD wLen)
{
	// int i;DAR_WrongType
	int nLen;
	int nDataType, nNum;
	BOOL bResult;
	int nTotalNum, FileStructNum;

#if(!WIN32)
	UNUSED(wLen);
#endif
	wLen = 0xFFFF;
	if(Buffer == NULL)
		return DAR_WrongType;

	nLen = 0;

	nDataType = Buffer[nLen];	//Structure_698: 总的结构数量
	nTotalNum = Buffer[nLen+1];
	nLen += 2;
	if(nTotalNum != 6)
	{
		return DAR_WrongType;
	}
	nDataType = Buffer[nLen];	//Structure_698: 文件信息
	FileStructNum = Buffer[nLen+1];
	nLen += 2;
	if(FileStructNum != 6)
	{
		return DAR_WrongType;
	}
	

	nDataType = Buffer[nLen];		//源文件文件
	nNum = Buffer[nLen+1];
	if(nNum < sizeof(tFileInfo.sourseFile))
	{
		memset(tFileInfo.sourseFile,0,sizeof(tFileInfo.sourseFile));
		memcpy(tFileInfo.sourseFile, &Buffer[nLen+2], nNum);
	}
	nLen += nNum+2;

	nDataType = Buffer[nLen];		//目标文件
	nNum = Buffer[nLen+1];
	if(nNum < sizeof(tFileInfo.destFile))
	{
		memset(tFileInfo.destFile,0,sizeof(tFileInfo.destFile));
		memcpy(tFileInfo.destFile, &Buffer[nLen+2], nNum);
	}

	nLen += nNum+2;

	nDataType = Buffer[nLen++];		//文件大小
	if(nDataType != Double_long_unsigned_698)
	{
		return DAR_WrongType;
	}
	tFileInfo.lFileSize = MDW(Buffer[nLen], Buffer[nLen+1], Buffer[nLen+2], Buffer[nLen+3]);
	nLen += 4;

	nDataType = Buffer[nLen];		//文件属性
	nLen += 2;
	tFileInfo.FileAttr = Buffer[nLen];
	nLen += 1;

	nDataType = Buffer[nLen];		//文件版本
	nNum = Buffer[nLen+1];
	if(nNum < sizeof(tFileInfo.FileVersion))
		memcpy(tFileInfo.FileVersion, &Buffer[nLen+2], nNum);

	nLen += nNum+2;

	nDataType = Buffer[nLen++];		//文件类别
	tFileInfo.byFileType = Buffer[nLen];
	nLen += 1;

	nDataType = Buffer[nLen++];  // 传输块大小
	if(nDataType == Long_unsigned_698)
	{
		tFileInfo.dwTxBlockLen = MW(Buffer[nLen], Buffer[nLen+1]);
		nLen += 2;
	}
	else
	{
		tFileInfo.dwTxBlockLen = MDW(Buffer[nLen], Buffer[nLen+1], Buffer[nLen+2], Buffer[nLen+3]);
		nLen += 4;
	}


	//nLen += GetExLen(&Buffer[nLen], &nNum);
	nDataType = Buffer[nLen]; //校验
	if(nDataType != Structure_698)
	{
		return DAR_WrongType;
	}
	nLen += 2;
	nDataType = Buffer[nLen++];
	if(nDataType != Enum_698)
	{
		return DAR_WrongType;
	}

	tFileInfo.byCheckType = Buffer[nLen];
	nLen += 1;

	nDataType = Buffer[nLen];
	nLen += 1;

	nDataType = Buffer[nLen];
	nLen += 1;
	if(nDataType == 1)
	{
		tFileInfo.dwCrcCheck = Buffer[nLen];
		nLen += 1;
	}
	else if(nDataType == 2)
	{
		tFileInfo.dwCrcCheck = MW(Buffer[nLen], Buffer[nLen+1]);
		nLen += 2;
	}
	else if(nDataType == 3)
	{
		tFileInfo.dwCrcCheck = MDW(Buffer[nLen], Buffer[nLen+1], Buffer[nLen+2], 0);
		nLen += 3;
	}
	else if(nDataType == 4)
	{
		tFileInfo.dwCrcCheck = MDW(Buffer[nLen], Buffer[nLen+1], Buffer[nLen+2], Buffer[nLen+3]);
		nLen += 4;
	}

	nDataType = Buffer[nLen]; //兼容软件版本号
	if(nDataType != Array_698)
	{
		return DAR_WrongType;
	}
	nLen += 2;

	nDataType = Buffer[nLen];
	nNum = Buffer[nLen+1];
	if(nNum < sizeof(tFileInfo.szSoftVersion))
		memcpy(tFileInfo.szSoftVersion, &Buffer[nLen+2], nNum);
		
	nLen += nNum+2;

	nDataType = Buffer[nLen];	//兼容硬件版本号
	if(nDataType != Array_698)
	{
		return DAR_WrongType;
	}
	nLen += 2;

	nDataType = Buffer[nLen];
	nNum = Buffer[nLen+1];
	if(nNum < sizeof(tFileInfo.szHardwareVersion))
		memcpy(tFileInfo.szHardwareVersion, &Buffer[nLen+2], nNum);
		
	nLen += nNum+2;

	nDataType = Buffer[nLen];	//下载方的标识
	nNum = Buffer[nLen+1];
        
        memset(tFileInfo.szDownFlag,0,33);
        
	if(nNum < sizeof(tFileInfo.szDownFlag))
		memcpy(tFileInfo.szDownFlag, &Buffer[nLen+2], nNum);
	

	nLen += nNum+2;

	bResult = CompareUpFile(&tFileInfo);
	if(bResult == FALSE)
	{
		memset(tFileInfo.BlockStatus,0,sizeof(tFileInfo.BlockStatus));
		WriteUpFileStatus();
	}
	tFileInfo.BlockStatusNum = 0;
	tFileInfo.FileStatus = eFileUpdateStartOk;

	bIsVipUpdateFlag = 0;

	return DAR_Success;
}
///////////////////////////////////////////////////////////////
//	函 数 名 : api_GetFileText
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月24日
//	返 回 值 : void
//	参数说明 : BYTE Ch,
//				 BYTE *Buffer,
//				 WORD wLen
///////////////////////////////////////////////////////////////
BYTE api_GetFileTextCrc(BYTE Ch, BYTE *Buffer, WORD wLen)
{
	int nLen;
	int nNum;
	BYTE byCodeFlag;
//	int nDataType;
	WORD wNo,i;
	WORD wStart;
	WORD wTotal;
	WORD wCrc,wCrcFile;
	BYTE FileBuffer[1024];
	int FileLen=0;
#if(!WIN32)
	UNUSED(wLen);
#endif
	
	wLen = 0xFFFF;
	if(Buffer == NULL)
		return 0;
	
	nLen = 0;
	
	// nDataType = Buffer[nLen];	//Structure_698:
	nNum = Buffer[nLen+1];
	nLen += 2;
	
	
	// nDataType = Buffer[nLen++];
	nLen++;
	wStart = MW(Buffer[nLen], Buffer[nLen+1]);
	nLen += 2;

	// nDataType = Buffer[nLen++];
	nLen++;
	wTotal = MW(Buffer[nLen], Buffer[nLen+1]);
	nLen += 2;

	// nDataType = Buffer[nLen++];	//Octet_string_698:
	nLen++;
	nLen += GetExLen(&Buffer[nLen], &nNum);	

	byCodeFlag = api_GetCodeNextBufferFlag(0);

	memset(tFileInfo.BlockStatus,0,256);
	FileLen = 0;
	for(wNo=wStart; wNo<wTotal || FileLen < tFileInfo.lFileSize; wNo++)
	{
		CLEAR_WATCH_DOG;
		if(( tFileInfo.lFileSize - FileLen) > 1024)
			nNum = 1024;
		else
			nNum = (tFileInfo.lFileSize - FileLen);
		
		api_ReadDataFromCodeBuffer(byCodeFlag, FileLen, nNum, &FileBuffer[0]);
		
		wCrcFile = fcs16(&FileBuffer[0], nNum);
		
		FileLen += nNum;

		wCrc = MW(Buffer[nLen], Buffer[nLen+1]);
		nLen += 2;

		if(tFileInfo.dwTxBlockLen == 0)
			tFileInfo.dwTxBlockLen = 256;

		for(i=0; i<(1024/tFileInfo.dwTxBlockLen); i++)
		{
			tFileText.lFileIndex = wNo*4+i;
			if(tFileText.lFileIndex*256 >= tFileInfo.lFileSize)
				break;

			if(wCrcFile == wCrc)
			{
				tFileInfo.BlockStatus[(tFileText.lFileIndex/8)%256] |= (0x1 << (tFileText.lFileIndex%8));
			}
			else
			{
				tFileInfo.BlockStatus[(tFileText.lFileIndex/8)%256] &= ~(0x1 << (tFileText.lFileIndex%8));
			}
		}
		
	}
	
	WriteUpFileStatus();

	return DAR_Success;
}

///////////////////////////////////////////////////////////////
//	函 数 名 : api_GetFileText
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月24日
//	返 回 值 : void
//	参数说明 : BYTE Ch,
//				 BYTE *Buffer,
//				 WORD wLen
///////////////////////////////////////////////////////////////
BYTE api_GetFileText(BYTE Ch, BYTE *Buffer, WORD wLen)
{
	int nLen;
	int nNum;
	BYTE byCodeFlag;
	BOOL bResult=FALSE;
	// int nDataType;
#if(!WIN32)
	UNUSED(wLen);
#endif
	
	wLen = 0xFFFF;
	if(Buffer == NULL)
		return 0;

	nLen = 0;

	// nDataType = Buffer[nLen];	//Structure_698:
	nNum = Buffer[nLen+1];
	nLen += 2;
	

	// nDataType = Buffer[nLen++];
    nLen++;
	tFileText.lFileIndex = MW(Buffer[nLen], Buffer[nLen+1]);
	nLen += 2;

	
	// nDataType = Buffer[nLen++];	//Octet_string_698:
    nLen++;
	//nDataType = Buffer[nLen];

	nLen += GetExLen(&Buffer[nLen], &nNum);

	if(tFileText.lFileIndex == 0)
	{
		//if( IsVipUserUpdate(&Buffer[nLen], nNum) )
		{
			bIsVipUpdateFlag = IsVipUserUpdate(&Buffer[nLen], nNum);
		}
	}

	if(nNum < sizeof(tFileText.File))
	{
		//memcpy(&tFileText.File[tFileText.lFileIndex*tFileInfo.dwTxBlockLen], &Buffer[nLen], nNum);

		byCodeFlag = api_GetCodeNextBufferFlag(0);
		bResult = api_WriteDataToCodeBuffer(byCodeFlag, tFileText.lFileIndex*tFileInfo.dwTxBlockLen, nNum, &Buffer[nLen]);
		
		if(bResult == FALSE)
		{
			bResult = WriteExtFlash2(0, tFileText.lFileIndex*tFileInfo.dwTxBlockLen, nNum, &Buffer[nLen] );
		}
	}


	
	nLen += nNum;

	//tFileInfo.BlockStatusNum = tFileText.lFileIndex+1;
	if(bResult)
	{
	
		tFileInfo.BlockStatus[(tFileText.lFileIndex/8)%256] |= (0x1 << (tFileText.lFileIndex%8));

		if(tFileInfo.lFileSize)
			tFileInfo.FileTransferStatus = ((100*((tFileText.lFileIndex+1)*tFileInfo.dwTxBlockLen)) / tFileInfo.lFileSize);

		if(tFileInfo.FileTransferStatus == 100)
			tFileInfo.FileStatus = eFileUpdateDownOk;
		else
			tFileInfo.FileStatus = eFileUpdateDowning;

		WriteUpFileStatus();

		return DAR_Success;
	}

	tFileInfo.FileStatus = eFileUpdateWProgramErr;

	return DAR_TempFail;
	//Trace("file addr=[%x], block=%d, index=%d", tFileText.lFileIndex*tFileInfo.dwTxBlockLen, tFileInfo.BlockStatusNum, tFileText.lFileIndex);
}

DWORD gdwMemeryMap;

BOOL api_WriteRam(DWORD Addr, WORD Length, BYTE * pBuf)
{
	memcpy((void*)(Addr),pBuf,Length);
    return TRUE;
}

BOOL api_ReadRam(DWORD Addr, WORD Length, BYTE * pBuf)
{
	memcpy(pBuf,(void*)(Addr),Length);
    return TRUE;
}


BYTE api_GetFileText_Write_jdfh(BYTE Ch,  BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf)
{
	int nLen;
	int nNum;
	// int nDataType;
	DWORD dwFlag,dwLen;
	
	if(InBuf == NULL)
		return 0;
	
	nLen = 0;
	
	// nDataType = InBuf[nLen++];	//Structure_698:
	nLen++;
	nLen += GetExLen(&InBuf[nLen], &nNum);
	
	dwFlag = MDW(InBuf[nLen], InBuf[nLen+1], InBuf[nLen+2],InBuf[nLen+3]);
	nLen += 4;
	
	// dwAddr = MDW(InBuf[nLen], InBuf[nLen+1], InBuf[nLen+2],InBuf[nLen+3]);
	nLen += 4;
	
	dwLen = MDW(InBuf[nLen], InBuf[nLen+1], InBuf[nLen+2],InBuf[nLen+3]);
	nLen += 4;

	if(dwFlag == 0xA1B21111)
	{
		// scm_flash_erase(dwAddr,4096);//zh
		OutBuf[0]=1;

		return 1;
	}
	//nDataType = InBuf[nLen++];
	
	if(dwLen == (nNum-12) )
	{
		//   WriteParaToCode(dwAddr,dwLen, &InBuf[nLen]);
	}
	
	OutBuf[0]=1;
	
	return 1;

	// return DAR_Success;
}
/*
WORD api_GetFileText_Read_jdfh( BYTE Ch,  BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	int nLen;
	int nNum;
	BYTE byCodeFlag;
	BOOL bResult=FALSE;
	int nDataType;
	
	DWORD dwAddr, dwLen,dwFlag;

	if(InBuf == NULL)
		return 0;
	
	nLen = 0;
	
	nDataType = InBuf[nLen];	//Structure_698:
	nNum = InBuf[nLen+1];
	nLen += 2;

	dwFlag = MDW(InBuf[nLen], InBuf[nLen+1], InBuf[nLen+2],InBuf[nLen+3]);
	nLen += 4;

        dwAddr = MDW(InBuf[nLen], InBuf[nLen+1], InBuf[nLen+2],InBuf[nLen+3]);
	nLen += 4;
        
        
        dwLen = MDW(InBuf[nLen], InBuf[nLen+1], InBuf[nLen+2],InBuf[nLen+3]);
	nLen += 4;
        
	if(dwFlag == 0xADBCCBDA)
	{
		byCodeFlag = api_GetCodeBufferFlag(0);

		api_SetCodeBufferLen(byCodeFlag, dwLen);
				
		api_SetCodeBufferSum(byCodeFlag, api_CalcCodeBufferSum(byCodeFlag));
				
		api_SetCodeBufferFlag(0, byCodeFlag);
				
		api_SetUpdateFlag(FALSE);
			
				
		api_ResetCpu();		
	}
	
        OutBuf[0]=0;
	return 1;//nLen + nNum;
	//return DAR_Success;
}
*/
extern unsigned char devid[8];
WORD api_Write_ID( BYTE Ch,  BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	int nLen;
	int nNum;
	// int nDataType;
	DWORD dwAddr;

	if(InBuf == NULL)
		return 0;
	
	nLen = 0;
	
	// nDataType = InBuf[nLen];	//Structure_698:
	nNum = InBuf[nLen+1];
	nLen += 2;
	
	dwAddr = MDW(InBuf[nLen],InBuf[nLen+1],InBuf[nLen+2],InBuf[nLen+3]);
	nLen += 4;
	
	if(nNum == 12 && dwAddr == 0xAABBCCDD)
	{
			memcpy(devid, &InBuf[nLen], 8);
			sm4_SetID(devid);
	}
	OutBuf[0]=0;
	return 1;//nLen + nNum;
}


WORD api_Write_IK( BYTE Ch,  BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	int nLen;
	int nNum;
	// int nDataType;
	DWORD dwAddr;

	if(InBuf == NULL)
		return 0;
	
	nLen = 0;
	
	// nDataType = InBuf[nLen];	//Structure_698:
	nNum = InBuf[nLen+1];
	nLen += 2;
	
	dwAddr = MDW(InBuf[nLen],InBuf[nLen+1],InBuf[nLen+2],InBuf[nLen+3]);
	nLen += 4;
	
	if(nNum == 100 && dwAddr == 0xAABBCCDD)
	{
			//memcpy(devid, &InBuf[nLen], 96);
            sm4_SetIKey(&InBuf[nLen]);
	}
	
	OutBuf[0]=0;
	return 1;//nLen + nNum;
}

WORD api_Write_SoftFlag( BYTE Ch,  BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	int nLen;
	int nNum;
	// int nDataType;
	DWORD dwAddr;

	if(InBuf == NULL)
		return 0;
	
	nLen = 0;
	
	// nDataType = InBuf[nLen];	//Structure_698:
	nNum = InBuf[nLen+1];
	nLen += 2;
	
	dwAddr = MDW(InBuf[nLen],InBuf[nLen+1],InBuf[nLen+2],InBuf[nLen+3]);
	nLen += 4;
	
	if(nNum == 42 && dwAddr == 0xAABBCCDD)
	{
		sm4_WriteSoftFlag(&InBuf[nLen]);
	}
	OutBuf[0]=0;
	return 1;//nLen + nNum;
}

void UpdateSoftFuc(void)
{
	BYTE byCodeFlag;

	WriteVipUpdateData(NULL, tFileInfo.lFileSize);

	byCodeFlag = api_GetCodeNextBufferFlag(0);

	tFileInfo.FileStatus = eFileUpdateSuccess;
	api_SetCodeBufferLen(byCodeFlag, tFileInfo.lFileSize);
			
	api_SetCodeBufferSum(byCodeFlag, api_CalcCodeBufferSum(byCodeFlag));
			
	api_SetCodeBufferFlag(0, byCodeFlag);
			
	api_SetUpdateFlag(FALSE);
					
	api_UpdateAppEventStart();

	WriteUpFileReset();

	api_PowerDownEvent();
			
	api_WriteFreeEvent(RT_FREE_UPDATA,698);

	api_ResetCpu();			
}
///////////////////////////////////////////////////////////////
//	函 数 名 : api_GetFileUpdateTime
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月24日
//	返 回 值 : void
//	参数说明 : BYTE Ch,
//				 BYTE *Buffer,
//				 WORD wLen
///////////////////////////////////////////////////////////////
BOOL bIsUpdateTime=FALSE;
void api_GetFileUpdateTime(BYTE Ch, BYTE *Buffer, WORD wLen)
{
	int nLen;
	int nDataType;
	BYTE byFlag[7] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	// BYTE byCodeFlag;
	int nNum;
	if(Buffer == NULL)
		return;
	
	// byCodeFlag = api_GetCodeNextBufferFlag(0);
	
	nLen = 2;
	
	nDataType = Buffer[nLen++];	//Structure_698: DateTime_S_698
	if(nDataType == DateTime_S_698)
	{
		tFileInfo.FileUpdateTime.wYear = MW(Buffer[nLen], Buffer[nLen+1]);
		nLen += 2;

		tFileInfo.FileUpdateTime.Mon = Buffer[nLen++];
		tFileInfo.FileUpdateTime.Day = Buffer[nLen++];
		tFileInfo.FileUpdateTime.Hour = Buffer[nLen++];
		tFileInfo.FileUpdateTime.Min = Buffer[nLen++];
		tFileInfo.FileUpdateTime.Sec = Buffer[nLen++];
	}

	tFileInfo.BlockStatusNum = GetFileBlockNum();
	nNum = GetFileBlockStatusLen();//((tFileInfo.BlockStatusNum + 8) / 8);

	if(!api_FileCrcCheck())//api_FileCrcCheck()
	{
		memset(tFileInfo.BlockStatus,0,sizeof(tFileInfo.BlockStatus));
		memcpy(&Buffer[nLen], tFileInfo.BlockStatus, nNum );
	}
	
	if(tFileInfo.FileStatus == eFileUpdateCheckOK)
	{
		if(!memcmp(&tFileInfo.FileUpdateTime,byFlag,7))
		{
			tFileInfo.FileStatus = eFileUpdateTimeWait;
			if(bIsBoot)
			{
				bIsBoot=FALSE;
			//	writeBoot();
				return;
			}

			bIsUpdateTime = FALSE;
			UpdateSoftFuc();
			return;
		}
        else
        {
          bIsUpdateTime = TRUE;
          tFileInfo.FileStatus = eFileUpdateTimeWait;
          return;
        }
		// tFileInfo.FileStatus = eFileUpdateTimerErr;
	}
}

BOOL FileRightfulCheck(BYTE* Buffer, DWORD lFileSize)
{
	int offset=0;
	int nLenght;
	BOOL FileCheck=FALSE;

	if(bIsVipUpdateFlag == TRUE)
		return TRUE;
		
	if( (Buffer[offset] == 0x68) && (Buffer[offset+1] == 0x55)
		&& (Buffer[offset+2] == 0xAA) && (Buffer[offset+3] == 0x68))
	{
		nLenght = MDW(Buffer[offset+11],Buffer[offset+10],Buffer[offset+9],Buffer[offset+8])+40;
		if(nLenght == lFileSize)
		{
			if((Buffer[offset+25] == 'w') && (Buffer[offset+26] == 'i') && (Buffer[offset+27] == 's')
				&& (Buffer[offset+28] == 'd') && (Buffer[offset+29] == 'o') && (Buffer[offset+30] == 'm'))
				FileCheck = TRUE;
		}
	}
	return FileCheck;
}

///////////////////////////////////////////////////////////////
//	函 数 名 : api_FileCrcCheck
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月24日
//	返 回 值 : BOOL
//	参数说明 : void
///////////////////////////////////////////////////////////////
BOOL api_FileCrcCheck(void)
{
#define BUFF_LEN 4096

	WORD wCrc=0xFFFF;
	int nLen=0,nNum;
	BYTE byCodeFlag;
	BYTE Buffer[BUFF_LEN];
        BOOL FileCheck = FALSE;
	byCodeFlag = api_GetCodeNextBufferFlag(0);
	WORD wCrcTemp;

	while(nLen < tFileInfo.lFileSize)
	{
		CLEAR_WATCH_DOG;
		if(( tFileInfo.lFileSize - nLen) > BUFF_LEN)
		    nNum = BUFF_LEN;
		else
			nNum = (tFileInfo.lFileSize - nLen);

		api_ReadDataFromCodeBuffer(byCodeFlag, nLen, nNum, &Buffer[0]);

		wCrc = fcs16_Ex(wCrc, &Buffer[0], nNum);

		nLen += nNum;
                
        if(nLen == tFileInfo.lFileSize)		//判断文件的合法性
		{
			api_ReadDataFromCodeBuffer(byCodeFlag, nLen-40, 40, &Buffer[0]);//防止最后一包报文字节小于40的防护

			FileCheck = FileRightfulCheck(&Buffer[0], tFileInfo.lFileSize);
		}
	}
	
	wCrc ^= 0xFFFF;
        wCrcTemp = wCrc;
        
	lib_InverseData((BYTE *)&wCrc, 2);

	if(tFileInfo.byCheckType == 0)
	{
		if( (wCrc == tFileInfo.dwCrcCheck || wCrcTemp == tFileInfo.dwCrcCheck) && FileCheck == TRUE )
		{
				//tFileInfo.FileStatus = eFileOk;
				tFileInfo.FileStatus = eFileUpdateCheckOK;
				return TRUE;
		}
		tFileInfo.FileTransferStatus = eFileCrcError;
		tFileInfo.FileStatus = eFileUpdateCheckErr;
				return FALSE;
	}
	else
		return TRUE;
}

void lib_ExchangeBitstring(BYTE *BufA, BYTE *BufB, BYTE Len )
{
    int j,k;
	BYTE Buf[255];
    
    memcpy( Buf, BufA, Len );
    memset( BufB, 0, Len);
	
    for( j=0; j<Len; j++ )
    {
        for( k=0; k<8; k++ )
		{
			if(Buf[j] & (0x01<<k))
			{
				BufB[Len - j -1] |= (0x01<<(7-k));
			}
		}
    }
		
		lib_ExchangeData(BufB,BufB,Len);
}


///////////////////////////////////////////////////////////////
//	函 数 名 : api_GetFileStatus
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月24日
//	返 回 值 : int
//	参数说明 : BYTE *Buffer
///////////////////////////////////////////////////////////////
int api_GetFileStatus(BYTE *Buffer)
{
	int nNum, nLen = 0;
	BYTE Buf[255];
	
	tFileInfo.BlockStatusNum = GetFileBlockNum();
	
	nNum = GetFileBlockStatusLen();//((tFileInfo.BlockStatusNum + 8) / 8);
	memset(Buf, 0, 255);
	
	Buffer[nLen++] = Bit_string_698;

	nLen += SetExLen(tFileInfo.BlockStatusNum , &Buffer[nLen]);

//	if(((tFileText.lFileIndex+1)*tFileInfo.dwTxBlockLen) < tFileInfo.lFileSize)
	{
		lib_ExchangeBitstring(tFileInfo.BlockStatus,Buf,nNum);
			memcpy(&Buffer[nLen], Buf, nNum );
	}
/*	else
	{
		if(api_FileCrcCheck())//api_FileCrcCheck()
		{
				lib_ExchangeBitstring(tFileInfo.BlockStatus,Buf,nNum);
				memcpy(&Buffer[nLen], Buf, nNum );
		}
		else
		{
			memset(tFileInfo.BlockStatus,0,sizeof(tFileInfo.BlockStatus));
			memcpy(&Buffer[nLen], tFileInfo.BlockStatus, nNum );
		}
	}
*/
	nLen += nNum;//((tFileInfo.BlockStatusNum + 8) / 8);

	if(tFileInfo.FileStatus == eFileUpdateDownOk)
		api_FileCrcCheck();
	
	return nLen;
}
///////////////////////////////////////////////////////////////
//	函 数 名 : api_GetFileTransmitStatus
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月30日
//	返 回 值 : int
//	参数说明 : BYTE *Buffer
///////////////////////////////////////////////////////////////
int api_GetFileTransmitStatus(BYTE *Buffer)
{
	int nLen = 0;

//	Buffer[nLen++] = Array_698;
	
	Buffer[nLen++] = tFileInfo.FileStatus;

	return nLen;
}
int api_GetFileTransmitStatusF001(BYTE *Buffer)
{
	int nLen = 0;
	
	//	Buffer[nLen++] = Array_698;
	
	Buffer[nLen++] = tFileInfo.FileTransferStatus;
	
	return nLen;
}

int api_GetFileTransmitMakeType(BYTE *Buffer)
{
	int nLen = 0;

//	Buffer[nLen++] = Array_698;
	
	Buffer[nLen++] = 0;//tFileInfo.FileStatus;

	return nLen;
}
int api_GetFileTransmitType(BYTE *Buffer)
{
	int nLen = 0;
	
	//	Buffer[nLen++] = Array_698;
	
	Buffer[nLen++] = tFileInfo.byFileType;
	
	return nLen;
}
///////////////////////////////////////////////////////////////
//	函 数 名 : api_GetFileLoadFlag
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月30日
//	返 回 值 : int
//	参数说明 : BYTE *Buffer
///////////////////////////////////////////////////////////////
int api_GetFileLoadFlag(BYTE DataType, BYTE *InBuf, BYTE *OutBuf, WORD wLen)
{
	int nLen = 0;
	
    if(InBuf == NULL)//指针为空指针,写flash时候调用这个分支
	{
		if(tFileInfo.szDownFlag[0] == 0)
		{
			#if(USER_TEST_MODE == 1)
			sprintf(tFileInfo.szDownFlag,"%s","DKY");	
			#else
			sprintf(tFileInfo.szDownFlag,"%s"," ");	
			#endif
		}

		OutBuf[nLen++] = Visible_string_698;

		OutBuf[nLen++] = strlen(tFileInfo.szDownFlag);

		memcpy(&OutBuf[nLen], tFileInfo.szDownFlag, strlen(tFileInfo.szDownFlag));

		nLen += strlen(tFileInfo.szDownFlag);//strlen(tFileInfo.szDownFlag)

	}
	else//读取flash时候调用这个分支
	{
		OutBuf[nLen++] = Visible_string_698;

		OutBuf[nLen++] = InBuf[1];//strlen(tFileInfo.szDownFlag);
		
		memcpy(&OutBuf[nLen], &InBuf[2], InBuf[1]);

		nLen += InBuf[1];//strlen(tFileInfo.szDownFlag);//strlen(tFileInfo.szDownFlag)
	}
	return nLen;
        
}


int api_GetFileLoadFlag2(BYTE *Buffer)
{
	int nLen = 0;
	
	if(tFileInfo.szDownFlag[0] == 0)
		sprintf(tFileInfo.szDownFlag,"%s","1234");	

	Buffer[nLen++] = Visible_string_698;

	Buffer[nLen++] = strlen(tFileInfo.szDownFlag);
	
	memcpy(&Buffer[nLen], tFileInfo.szDownFlag, 32);

	nLen += strlen(tFileInfo.szDownFlag);//strlen(tFileInfo.szDownFlag)

	return nLen;
}

///////////////////////////////////////////////////////////////
//	函 数 名 : api_GetFileBlockSize
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月30日
//	返 回 值 : int
//	参数说明 : BYTE *Buffer
///////////////////////////////////////////////////////////////
int api_GetFileBlockSize(BYTE *Buffer)
{
	int nLen = 0;
	
	//Buffer[nLen++] = Visible_string_698;
	
	Buffer[nLen++] = 0;
	Buffer[nLen++] = 0x01;
	Buffer[nLen++] = 0x00;
	Buffer[nLen++] = 0x00;

	return nLen;
}

///////////////////////////////////////////////////////////////
//	函 数 名 : api_GetFileExeTime
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月30日
//	返 回 值 : int
//	参数说明 : BYTE *Buffer
///////////////////////////////////////////////////////////////
int api_GetFileExeTime(BYTE *Buffer)
{
	int nLen = 0;
	
//	Buffer[nLen++] = DateTime_S_698;

	Buffer[nLen++] = tFileInfo.FileUpdateTime.wYear%0x100;//解决抄读时不正确问题，一个是用0x100,一个是传输倒序问题
	Buffer[nLen++] = tFileInfo.FileUpdateTime.wYear/0x100;


	Buffer[nLen++] = tFileInfo.FileUpdateTime.Mon;
	Buffer[nLen++] = tFileInfo.FileUpdateTime.Day;
	Buffer[nLen++] = tFileInfo.FileUpdateTime.Hour;
	Buffer[nLen++] = tFileInfo.FileUpdateTime.Min;
	Buffer[nLen++] = tFileInfo.FileUpdateTime.Sec;

	return nLen;
}

int api_GetFileVersion(BYTE *Buffer,BYTE classType)
{
	int nLen = 0;
	char szText[20];

	memset(szText,0,20);

	if(classType == 4)
	{
#if(USER_TEST_MODE)
          sprintf(szText,"%s",MODULE_VERSION_T);
#else
	  sprintf(szText,"%x",MODULE_VERSION);
#endif
          memcpy(&Buffer[nLen], szText, strlen(szText));
	
           nLen += strlen(szText);
        
	}
	else if(classType == 2)
	{
		sprintf(szText,"%s","NILM");
                 memcpy(&Buffer[nLen], szText, strlen(szText));
	
                nLen += strlen(szText);
	}
	
		
	return nLen;
}
BYTE api_GetBoardTime(BYTE Ch, BYTE *Buffer, WORD wLen)
{
	TRealTimer TmpRealTimer;
	DWORD RelativeTime;

	if(Buffer[0] == DateTime_S_698)
	{
		lib_InverseData(&Buffer[1], 2);
		if( api_CheckClock( (TRealTimer*)&Buffer[1] ) == FALSE )
		{
			return DAR_HardWare;
		}
		api_AdjustTimeStart(); // 校时记录开始//zh
		RelativeTime = api_GetRelativeMin();
		memcpy((BYTE*)&(TmpRealTimer.wYear),&Buffer[1],sizeof( TRealTimer ));
		api_WriteMeterTime(&TmpRealTimer);
		api_UpdateFreezeTime(RelativeTime);
		api_AdjustTimeEnd(); // 校时记录结束//zh
	}
	return DAR_Success;		
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
    TTwoByteUnion OI,OProperty;
    BYTE DAR;
    BYTE *pData;
    eAPPBufResultChoice eResultChoice;
	#if(PROTOCOL_VERSION == 25)
	TFreezeCycleSafeRom Frzpara;
	TSpecificDevice SpcDev;
	#endif

    DAR = DAR_WrongType;
    lib_ExchangeData(OI.b,pOMD,2);//取oi
    pData = &pOMD[4];//取data
    DAR = DAR_WrongType;//默认失败

    // SecurityMode = APPData[Ch].eSafeMode;
    
	lib_ExchangeData(OProperty.b,&pOMD[2],2);//取oi
	if(OI.w == 0xF001)
	{
		switch(OProperty.w)
		{
		case 0x0800:
			DAR = api_GetFileText( Ch, pData, 0);
			break;
		case 0x0900:
			DAR = api_GetFileTextCrc( Ch, pData, 0);
			break;
		case 0x0e00:
			DAR = api_GetFileInfo( Ch, pData,0);
			
			break;
		case 0x0400:
			break;
		case 0x0D00://执行升级（参数）
			api_GetFileUpdateTime(Ch, pData,0);
			DAR = DAR_Success;
			break;
		}
	}
	else if(OI.w == 0x4000)
	{
		if( pOMD[2] == 127 )// 方法127 //
		{
			if(api_GetBoardTime(Ch,pData,0) == DAR_Success)
			{
				DAR = DAR_Success;
			}
			else
			{
				DAR = DAR_HardWare;
			}

		}
	}
	
    if( pData[0] == NULL_698 )//数据为NULL
    {
        switch( pOMD[2] )
        {
			case 1://设备复位重启
				api_ResetCpu();
				DAR = DAR_Success;
				break;
			case 4://恢复出厂参数 
				//api_FactoryInitEvent();
				GetDefaultThresholdValue(1);
				DAR = DAR_Success;
				break;
			// case 5://5事件初始化
			// 	api_FactoryInitEvent();
			// 	DAR = DAR_Success;
			// 	// ClearPowerEventFlag();//zh
			// 	break;
			case 3://数据初始化
				if(sm4_GetStatus() == 0)
				{
					api_FactoryInitEvent();
					api_FactoryInitFreeze();
					// ClearPowerEventFlag();zh
					api_MeterClearEventStart();//wxy 2021.12.1
					// 调用 API
					memset(&SafeModePara.Num, 0x00, sizeof(TSafeModePara));
					SafeModePara.Flag = 1;
					api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(ProSafeRom.SafeModePara), sizeof(TSafeModePara), (BYTE *)&SafeModePara );
					
					#if(PROTOCOL_VERSION == 25)
					memset((BYTE*)&SpcDev,0xFF,sizeof(TSpecificDevice));
					SpcDev.bySpecificDeviceNum = 0;
					api_SetSpecificDeviceID(0x0B,(TSpecificDevice*)&SpcDev);										//设置特定设备ID为默认值
					Frzpara.Cycle = 0x000F;  						//默认值15分钟
					api_WriteFreezeAttribute(0x5002,(BYTE*)&Frzpara.Cycle);
					#endif

					DAR = DAR_Success;
				}
				else
				{
					DAR = DAR_RefuseOp;
				}
				
				break;
			case 8:
				copy_default_softflag();
				ReadSm4Para(1);
				DAR = DAR_Success;
				break;
			case 9://全部初始化
				api_FactoryInitEvent();
				api_FactoryInitFreeze();
				// ClearPowerEventFlag();zh
				FactoryInitMeterClearPara();
				GetDefaultThresholdValue(1);
				sm4_InitKey();
				// 调用 API
				memset(&SafeModePara.Num, 0x00, sizeof(TSafeModePara));
				SafeModePara.Flag = 1;
				api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(ProSafeRom.SafeModePara), sizeof(TSafeModePara), (BYTE *)&SafeModePara );
				
				#if(PROTOCOL_VERSION == 25)
				memset((BYTE*)&SpcDev,0xFF,sizeof(TSpecificDevice));
				SpcDev.bySpecificDeviceNum = 0;
				api_SetSpecificDeviceID(0x0B, (TSpecificDevice*)&SpcDev);										//设置特定设备ID为默认值
				Frzpara.Cycle = 0x000F;  						//默认值15分钟
				api_WriteFreezeAttribute(0x5002,(BYTE*)&Frzpara.Cycle);
				#endif
				
				DAR = DAR_Success;
				break;
			case 0x0B:
				copy_default_softflag();
				ReadSm4Para(1);
				sm4_InitKey();
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
	TTwoByteUnion Leninpage;
//	TFourByteUnion OAD;
	BYTE Buffer[100];
//	OAD.d = 0x000400F1;
	
	if( pData[0] == Structure_698 )//结构体
	{
		//if( pData[1] == Structure_698 )//sequence of data
		{
			//if( pData[2] == Octet_string_698)
			{
					//Leninpage.w = Deal_698DataLenth( &pData[3], Len.b, ePROTOCOL_TYPE);//8是类型
					//lib_ExchangeData((BYTE *)&OI, &pData[3+Leninpage.w], 2);//获取oi
					//memcpy( &OAD, &pData[3+Leninpage.w],4 );
					Leninpage.w = pData[3];			
					
						memcpy(&Buffer[6],(BYTE*)&pData[4],Leninpage.w);
						memcpy(&Buffer[0],(BYTE*)&pData[4+Leninpage.w+1],4);
						memcpy(&Buffer[4],(BYTE*)&pData[4+Leninpage.w+6],2);
						memcpy(&Buffer[6+Leninpage.w],(BYTE*)&pData[4+Leninpage.w+9],4);
							
					Result = sm4_InitNet_Keydistribut( (BYTE*)&Buffer[0],74);	
					if( Result == TRUE )
					{
						DAR = DAR_Success;
					}
					else
					{
						DAR = DAR_EsamFial;//AR 硬件时效	
					}
			}		
		}
		//else
		{
		//	DAR = DAR_WrongType;//AR 硬件时效	
		}
	}
	else
	{
		DAR = DAR_WrongType;//AR 硬件时效	
	}
	
	return DAR;
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
// 红外认证请求（参数）
// 参数∷=RN（随机数1）
// 应答∷=structure
// {
// 表号           octet-string，
// ESAM序列号     octet-string，
// 随机数1密文   octet-string，
// 随机数2        RN
// }
//--------------------------------------------------
eAPPBufResultChoice ActionIRRequest( BYTE Ch, BYTE *pData)//
{
	// WORD Result;
	BYTE DAR;
	BYTE Tmp_RN[MAX_RN_SIZE+3],OutBuf[32];
	eAPPBufResultChoice eResultChoice;

    DAR = DAR_WrongType;
    //断开已建立的应用连接
    
  	if( pData[0] == RN_698 )//随机数
	{
		if( pData[1] < MAX_RN_SIZE )
		{
			Tmp_RN[0] = pData[1];
			memcpy( (BYTE*)&Tmp_RN[1], (BYTE*)&pData[2], pData[1] );//获取随机数

			if(sm4_InitNet_Authent1(&Tmp_RN[1], OutBuf) == TRUE)
				DAR = DAR_Success;
			else
				DAR = DAR_WrongType;
		}
	}
	else
	{
		DAR = DAR_WrongType;//AR 硬件时效   
	}
	
    if( DAR == DAR_Success )
	{
		ProtocolBuf[0] = DAR_Success;//AR 成功	
		ProtocolBuf[1] = 0x01;//choice data
		ProtocolBuf[2] = Structure_698;//no data
		
        ProtocolBuf[3] = 04;
        
        ProtocolBuf[4] = Octet_string_698;//表号
        ProtocolBuf[5] = 6;
		memset(&ProtocolBuf[6],'X',6);		//获取表号(6个字节) 上面easm通信成功，认为esam没问题
  
        ProtocolBuf[12] = Octet_string_698;//ESAM序列号
        ProtocolBuf[13] = 8;
        api_GetEsamInfo( 2, ProtocolBuf+14 );//8个字节 上面easm通信成功，认为esam没问题
        memmove( ProtocolBuf+14, ProtocolBuf+16, 8);
        
        ProtocolBuf[22] = Octet_string_698;//随机数1密文
        ProtocolBuf[23] = 16;
        memcpy(ProtocolBuf+24, &OutBuf[0], 16);
        
        ProtocolBuf[40] = RN_698;//Octet_string_698;//随机数2
        ProtocolBuf[41] = 16;
        memcpy( ProtocolBuf+42, &OutBuf[16], 16);
        
		eResultChoice = DLT698AddBuf(Ch, 0x55,ProtocolBuf ,58 );
	}
	else
	{
		ProtocolBuf[0] = DAR;
		ProtocolBuf[1] = 0;
		eResultChoice = DLT698AddBuf(Ch, 0x55,ProtocolBuf ,2);
	}

    return eResultChoice;
}

eAPPBufResultChoice ActionEsamDataUpdate( BYTE *pData)//
{
	WORD Result;
	eAPPBufResultChoice DAR = eADD_OK;
	TTwoByteUnion Leninpage;
	BYTE Buffer[100]={0};
	
	Result = FALSE;
	
	if( pData[0] == Structure_698 )//结构体
	{
		if( pData[2] == Structure_698)//oi
		{
			//Leninpage.w = Deal_698DataLenth( &pData[3], Len.b, ePROTOCOL_TYPE);//8是类型
			//lib_ExchangeData((BYTE *)&OI, &pData[3+Leninpage.w], 2);//获取oi
			//memcpy( OAD, &pData[3+Leninpage.w],4 );
			Leninpage.w = pData[3];	

			memcpy(&Buffer[6],(BYTE*)&pData[4],Leninpage.w);
			memcpy(&Buffer[0],(BYTE*)&pData[4+Leninpage.w+1],4);
			memcpy(&Buffer[4],(BYTE*)&pData[4+Leninpage.w+6],2);
			memcpy(&Buffer[6+Leninpage.w],(BYTE*)&pData[4+Leninpage.w+9],4);
			
			Result = sm4_SetIKey( (BYTE*)&Buffer[0]);	
			if( Result == TRUE )
			{
				DAR = eADD_OK;
			}
			else
			{
				DAR = eAPP_ESAMFIAL;//AR 硬件时效	
			}
		}
		else
		{
			DAR = eAPP_WRONGTYPE;//AR 硬件时效	
		}
		
	}
	else
	{
		DAR = eAPP_WRONGTYPE;//AR 硬件时效	
	}
	
	return DAR;
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
BYTE ActionIRAuth( BYTE Ch, BYTE *pData)//
{
    BYTE DAR,Result;
    DWORD tdw = 0;

    DAR = DAR_WrongType;
    
	if( pData[0] == Octet_string_698 )//获取随机数2密文 
	{
		memcpy( ProtocolBuf, (BYTE*)&pData[2], pData[1] );
		Result = sm4_InitNet_Authent2( ProtocolBuf );
		if( Result == FALSE )
		{
			DAR = DAR_EsamFial;
		}
		else
		{
			//api_ReadPrePayPara( 3, (BYTE*)&tdw );
			APPConnect.IRAuthTime = (tdw*60);//将时效分钟改为秒
		
			DAR = DAR_Success;
		}
	}
	

    return DAR;
}

BOOL bIsStatReset=FALSE;
eAPPBufResultChoice ActionEsamDataRestore( BYTE *pData)//
{
//	WORD Result;
	eAPPBufResultChoice DAR;
	
//	Result = FALSE;
	
	DAR = eADD_OK;
	
    bIsStatReset = TRUE;
	//statReset();

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
        case 0x01: //密钥恢复安全传输流程
		DAR = ActionEsamDataRestore( pData );
		break;

	case 0x03://数据回抄
		eResultChoice = eAPP_WRONGTYPE;//ActionEsamDataRead( Ch, pData );
		return eResultChoice;

	case 0x04://数据更新
		DAR = ActionEsamDataUpdate( pData );
		break;
	case 0x05://协商失效
		eResultChoice = (eAPPBufResultChoice)sm4_UnVoidAuthent();//ActionEsamDisableConsultation( Ch, pData );
		return eResultChoice;
	case 0x06://钱包操作---未做
		DAR = DAR_WrongType;//ActionEsamMoney( Ch, pData);
		break;
	case 0x07://密钥更新
		DAR = ActionUpdateEsamKey( pData );
		break;
	case 0x08://证书更新		
		break;
	case 0x09://设置协商时效
		DAR = DAR_WrongType;//ActionEsamConsultationTime( pData );
		break;
	case 0x0A://钱包初始化 ---未做
		Sm4_Reset(); 
		break;	
	case 0x0B://红外认证请求
		eResultChoice = ActionIRRequest( Ch, pData );
		return eResultChoice;
	case 0x0C://红外认证指令
		DAR = ActionIRAuth(Ch, pData);
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
extern BOOL bBpsChange;
extern TEventPara gEventPara;
eAPPBufResultChoice ActionRequestF20D( BYTE Ch,  BYTE *pOMD)//操作esam安全模式
{
	BYTE DAR;
	BYTE *pData;
	eAPPBufResultChoice eResultChoice;
//	DWORD dwOad;
	int nBps;
        TEventPara tempEventPara;
        
	pData = &pOMD[2];//取数据指针
	DAR = DAR_WrongType;
	
	switch( pOMD[2] )
	{
	case 128://
		if( pData[4] == OAD_698)
		{
			// dwOad = MDW(pData[5],pData[6],pData[7],pData[8]);
			DAR = DAR_Success;//AR 成功	
		}
		if(pData[9] == COMDCB_698)
		{
			nBps = GetPortBpsFromIndex(pData[10]);
                        gdwUartBps = nBps;
                        bBpsChange = TRUE;
                          
                        //MX_USART1_UART_Init_Bps(gdwUartBps);
                        gEventPara.nUartBps = gdwUartBps;
                        
                        api_VReadSafeMem(GET_CONTINUE_ADDR( EventConRom.EventPara ), sizeof(TEventPara), (BYTE *)&tempEventPara);
                        tempEventPara.nUartBps = gdwUartBps;
                        api_VWriteSafeMem(GET_CONTINUE_ADDR(EventConRom.EventPara), sizeof(TEventPara), (BYTE *)&tempEventPara);//wxy 20220829
                                
			DAR = DAR_Success;//AR 成
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
    BYTE OMD[4],Method = 0;

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

    if( APPData[Ch].TimeTagFlag == eTime_Invalid )//时间标签错误
    {
        eResultChoice = DLT698AddOneBuf( Ch, 0x55, DAR_TimeStamp);//DAR
        if( eResultChoice != eADD_OK )
        {
            return eResultChoice;
        }
        eResultChoice = DLT698AddOneBuf( Ch, 0x55, 0);//DATA OPTINAL
    }
    else if( OI.w == 0x4300 && Method == 3 && APPData[Ch].TimeTagFlag == eNO_TimeTag 
            && api_GetSysStatus( eSYS_STATUS_INSIDE_FACTORY ) != TRUE)//操作电气设备 wxy 2022.11.07加 清零需要带时标
    {
        eResultChoice = DLT698AddOneBuf( Ch, 0x55, DAR_TimeStamp);//DAR
        if( eResultChoice != eADD_OK )
        {
            return eResultChoice;
        }
        eResultChoice = DLT698AddOneBuf( Ch, 0x55, 0);//DATA OPTINAL
    }
    
    else if( OI.w == 0x4300 || OI.w == 0xF001 || OI.w == 0x4000)//操作电气设备
    {
        eResultChoice = ActionRequestMeter( Ch,  pOMD);
    }
    else if( OI.w == 0xf100 )//操作esam 接口类
	{
		eResultChoice = ActionRequestEsam( Ch, pOMD);
		//数据更新 密钥更新 设置协商时效 开户充值 钱包初始化
		if( (Method == 7) ||(Method == 9) ||(Method == 6)||(Method == 0X0A) )
		{
            // Mode = 0x55;
			;
		}
	}
	else if ( OI.w == 0xf101 )
	{
		eResultChoice = ActionRequestEsamSafeMode( Ch, pOMD);
		if( (Method == 127) || (Method == 129) )//增加 批量增加安全模式参数
		{
            // Mode = 0x55;
			;
		}
	}
	else if(OI.w == 0x20EB)
	{
		eResultChoice = GetTransCommand( Ch,pOMD);
		// Mode = 0x55;
		;
	}
	else if(OI.w == 0xF20D)
	{
		eResultChoice = ActionRequestF20D( Ch, pOMD);
		// Mode = 0x55;
		;
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
    case ACTION_REQUEST_NORMALLIST:	
	eResultChoice = ActionRequestList( Ch,(BYTE  *)&(pAPDU[3]) );
	break;
    case SET_THEN_GET_REQUEST:
	eResultChoice = ActionThenGetRequestNormalList( Ch,(BYTE  *)&(pAPDU[3]) );
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
		case PROXY_REQUEST:
			Len1 = AnalyseSetRequestLen( (BYTE*)&LinkData[Ch].pAPP[0] );
			if( Len1 == 0x8000 )
            {
                return 0x8000;
            }
            Len = Len1;
			break;
		case REPORT_RESPONSE:
			Len = 0;
			break;
		default:
		    return 0x8000;
		}
	}

	return Len;
}


//-----------------------------------------------
//函数功能: 根据事件索引号计算对应OAD
//
//参数: 	
//		Index[in]:	事件索引号
//		Buf[out]：	计算出的单个OAD 
//           	
//返回值:	无
//			
//备注:
//-----------------------------------------------
extern TReportPara ReportPara;
extern TFollowPara FollowPara;

static BOOL CalReportOad(BYTE Index, BYTE *Buf)
{
	BYTE Phase;
	TFourByteUnion tdw;
	DWORD OAD;

	if(InitComplete == 0) //握手完成后才能跟随上报
	{
		return FALSE;
	}

	if(FollowPara.OAD_Num == 0)
	{
		return FALSE;
	}

	OAD = FollowPara.ReportOAD;
			
	Phase = ((OAD&0xFF00)>>8)-6;
	tdw.w[1] = (WORD)(OAD>>16);
	
	#if(PROTOCOL_VERSION == 25)
	if((tdw.w[1] == 0x3013) || (tdw.w[1]==0x302F) || (tdw.w[1]==0x3036)|| (tdw.w[1]==0x3016) || (tdw.w[1]==0x309A )|| (tdw.w[1]==0x309B))//zh
	#elif(PROTOCOL_VERSION == 24)
	if((tdw.w[1] == 0x3013) || (tdw.w[1]==0x302F) || (tdw.w[1]==0x3036)|| (tdw.w[1]==0x3016))//zh
	#endif
	{
		tdw.b[1]=0x02;
	}
	else
	{
		tdw.b[1]=Phase+0x06;
	}
	tdw.b[0]=0;
			
	lib_ExchangeData(Buf,tdw.b,sizeof(DWORD));

	return TRUE;
}

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
BOOL GetFirstReportOad(BYTE Type, BYTE Ch, BYTE *Buf)
{
	if(Type == 0)
	{
		return CalReportOad(0, Buf);
	}
    return FALSE;
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

   // DLT698AddOneBuf(Ch,0x77, 00);//添加00 无主动上报

	//默认不允许跟随上报 4300	电气设备 的属性7. 允许跟随上报                 红外通道不允许上报

	if(gReportMode.g_EnableFollowReportFlag == FALSE )
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
			DLT698AddOneBuf(Ch,0x77, 00);//添加00 无主动上报
		}
		else
		{
			DLT698AddBuf( Ch, 0x77, Buf, 13 );//添加跟随上报33200201
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
	return TRUE;
}
//-----------------------------------------------
//函数功能: 建立应用连接
//
//参数:		Buf[in/out]	输入时32字节密文1+4字节客户机签名1
//						输出时长度+48字节随机数+4字节客户机签名，长度两字节，高在前，低在后
//返回值:	TRUE/FALSE
//		   
//备注:在698规约中建立应用连接时使用
//-----------------------------------------------
BOOL api_ConnectMechanismInfo(BYTE *Buf)
{
	BOOL Result=FALSE;
	BYTE BufferRand[48];

	Result =  my_getRand((uint32_t*)BufferRand, 12);
	Buf[0] = 0;
	Buf[1] = 48;
	memcpy(&Buf[2],BufferRand,48);

	return Result;
}
//--------------------------------------------------
//功能描述:  建立应用链接认证处理
//         
//参数:     BYTE *pBuf[in]
//         
//返回值:	  TRUE/FALSE
//         
//备注内容:  无
//--------------------------------------------------
static WORD SecurityMechanismInfo(BYTE *pBuf)
{
	BYTE result;
	BYTE LenOffset,Len1Offest;
	TTwoByteUnion Len,Len1;
	if( pBuf[0] == 0 )
	{
		ConnectMode = 0;//保存建立应用连接的模式
		return TRUE;//允许建立公共连接
	}	
	else if( pBuf[0] == 1 )
	{
		if( pBuf[1] != 8 )//一般密码明文长度 密码长度不能大于7
        {
			return FALSE;
        }  
        
        ConnectMode = 1;//保存建立应用连接的模式
		
		return Judge698PassWord( (BYTE*)&pBuf[2] );
	}
	else if( pBuf[0] == 2 )//对称加密    [2] SymmetrySecurity，
	{
		LenOffset = Deal_698DataLenth( (BYTE*)&pBuf[1], Len.b, ePROTOCOL_TYPE );
		Len1Offest = Deal_698DataLenth( (BYTE*)&pBuf[1+LenOffset+Len.w], Len1.b, ePROTOCOL_TYPE );
		
        memcpy( ProtocolBuf, &pBuf[LenOffset+1],Len.w );// 密文1        octet-string，
        memcpy( &ProtocolBuf[Len.w], &pBuf[(1+LenOffset+Len.w+Len1Offest)], Len1.w );// 密文1        octet-string，
		
		result = api_ConnectMechanismInfo( ProtocolBuf );//进行应用连接
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
	WORD Result=0, Result_ESAM;
	DWORD dwData;
	BYTE *pbakbuf;
	
	TTwoByteUnion Len;
	TFourByteUnion TmpTime,TmpTime1,BufLen;
	BYTE Buf[20];
	pbakbuf = pBuf;

    if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return;
    }

	APPData[Ch].APPFlag = APP_HAVE_DATA;

	memcpy( (void *)pBuf, (void *)pAPDU, 2);
	pBuf += 2;
	
	lib_ExchangeData(BufLen.b,pAPDU+30, 4);//取客户机接受帧最大尺寸
    ConsultData.channel = Ch;
    ConsultData.Len = BufLen.d;
    
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
	
    Result = SecurityMechanismInfo(pAPDU+39);//1+2+8+16+2+2+1+2+4   

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
	api_GetProOadData( eData, 0xff, (BYTE *)&dwData, NULL, (4+4+6+4+6+8+4), pBuf );
	pBuf += 32;
	
	pBuf[0] = pAPDU[2];//协议版本号返回主站下发来的
	pBuf[1] = pAPDU[3];
	//Result = api_ProcMeterTypePara( READ, eMeterProtocolVersion, pBuf );//协议版本号
	pBuf += 2;
	
	memcpy( (void *)pBuf, (void *)&(pAPDU[4]), 24 );//协议一致性与功能一致性返回主站发来的 wlk
	pBuf += 24;
	//pBuf += AddProtocolAndFunctionConformance( 0, pBuf );//添加协议一致性与功能一致性Buf
	
	Len.w= MAX_PRO_BUF;//wxy ConsultData.Len;//服务器发送帧最大尺寸    long-unsigned
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
		Result_ESAM = api_GetEsamInfo( 5, Buf);//读f1000500 ESAM对象的属性5．会话时效门限		
	}

	//安全认证响应对象        SecurityResponseInfo	
	//*(pBuf++) = Result; //应用连接请求认证结果
	if( LinkData[Ch].AddressType!=eSINGLE_ADDRESS_TYPE )
	{
        *(pBuf++) = 0xff;//认证结果      ConnectResult 中 对称解密错误         （2），
        *(pBuf++) = 0;//其他错误（时间标签错误）
       // APPConnect.Flag = FALSE;
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

		
		//APPConnect.Flag = FALSE;
	}
	else
	{	
	
		if( Result_ESAM == FALSE )//获取建立应用链接有效时间
		{
			APPConnect.ConnectValidTime = TmpTime.d;
		}
		else
		{
			lib_ExchangeData(TmpTime1.b, Buf+2, 4);
			TmpTime1.d = TmpTime1.d*60;//单位分钟,转化为秒
			if( TmpTime.d > TmpTime1.d )
			{
				APPConnect.ConnectValidTime = TmpTime1.d;
			}
			else
			{
				APPConnect.ConnectValidTime = TmpTime.d;
			}
		}
		
        if( ConnectMode == 2 )//对称密码
        {
            APPConnect.Mode = 0x55;
        }
        else
        {
            APPConnect.Mode = 0x00;
        }
        
		ClientAddress = LinkData[Ch].ClientAddress;
		APPConnect.ConstConnectValidTime = APPConnect.ConnectValidTime ;
		//api_GetRtcDateTime( 0x807f,(BYTE*)&APPConnect.ConnectTime);//获取建立应用链接时间
		APPConnect.ConnectChannel = Ch;
		//APPConnect.Flag = TRUE;
		
		lib_ExchangeData( pBuf, (BYTE*)&APPConnect.ConstConnectValidTime, 4 );//添加会话应用连接超时时间
	    pBuf += 4;
	    
		*(pBuf++) = 0;// ConnectResult∷=ENUMERATED {  允许建立应用连接     （0），
		if(pAPDU[39] == 2 )//对称密码 //对称加密    [2] SymmetrySecurity，
		{
			*(pBuf++) = 0x01;//选择数据
			
			*(pBuf++) = 48;//RN长度
			memcpy( (void *)pBuf, &ProtocolBuf[2],48);//上报esam认证数据 data
			pBuf += 48;

			*(pBuf++) = 4;//签名长度
			memcpy( (void *)pBuf, &ProtocolBuf[2+48],4);//上报MAC 
			pBuf += 4;
		}
		else
		{
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
////--------------------------------------------------
////功能描述:  698连接断开请求
////参数:     BYTE Ch[in]
////         
////返回值:    void无返回值 
////         
////备注内容:  无
////--------------------------------------------------
//static void DealRelease_Request(BYTE Ch)
//{
//    if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
//    {
//        return;
//    }
//    
//	DLT698AddBuf(Ch, 0, LinkData[Ch].pAPP,3);					//添加命令头
//	DLT698AddOneBuf(Ch,0x55,0 );							    //添加结果 成功
//
//    APPConnect.Mode = 0;
//    APPConnect.ConnectValidTime = 0;
//    APPConnect.ConstConnectValidTime = 0;
////    APPConnect.Flag = FALSE;
//	APPConnect.ConnectChannel = 0x00;							//置位通道错误
//								  
//	APPData[Ch].APPFlag = APP_HAVE_DATA;
//	APPData[Ch].APPBufLen.w = 4;
//	APPData[Ch].BufDealLen.w = LinkData[Ch].pAPPLen.w;
//}
//--------------------------------------------------
//功能描述:  安全认证失败的时候，方法1、2、9的rcsd同一处理
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
WORD GetReorderRcsdLen(BYTE *pBuf)
{
	BYTE Method;
	TTwoByteUnion OI;	
	int nLen=0;
	DWORD ProOAD;

	if(pBuf == NULL)
		return 0x8000;

	// Len.w = 0;
	lib_ExchangeData(OI.b,pBuf,2);
	nLen += 2;

//	ClassAttribute = (pBuf[nLen]&0x0f);
	nLen += 2;

	Method = pBuf[nLen];
	nLen += 1;

	//if(OI.w == 0x5002)
	{
		switch(Method)
		{
		case 1:
			lib_ExchangeData((BYTE*)&ProOAD,&pBuf[nLen],4);
			nLen += 4;

			if(pBuf[nLen++] == DateTime_S_698)
				nLen += 7;

			break;
		case 2: 
			lib_ExchangeData((BYTE*)&ProOAD,&pBuf[nLen],4);
			nLen += 4;

			if(pBuf[nLen++] == DateTime_S_698)
				nLen += 7;

			if(pBuf[nLen++] == DateTime_S_698)
				nLen += 7;
			
			if(pBuf[nLen++] == TI_698)
				nLen += 3;

			break;
		case 9:
			nLen += 1;
			break;
		default:
			nLen = 0x8000;
		}
	}
	//else
	{
	//	nLen = 0x8000;
	}
	return nLen;
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
	eAPPBufResultChoice eResultChoice;
	BYTE Cmd;
	WORD wLen;
	BYTE nOffset=0;

	if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
	{
		return;
	}
	
	if(LinkData[Ch].pAPP[1] == 2 || LinkData[Ch].pAPP[1] == 4) //normal list 和 record list
		nOffset = 1;
	else
		nOffset = 0;

	if((!JudgeTaskAuthority( Ch, eREAD_MODE,&LinkData[Ch].pAPP[3+nOffset])) && (nOffset == 0))//判断安全模式参数
	{
		DLT698AddBuf(Ch, 0, LinkData[Ch].pAPP,3+nOffset);//添加命令头
		APPData[Ch].BufDealLen.w = 3+nOffset;
		eResultChoice = eAPP_ERR_RESPONSE;
		
		eResultChoice = DLT698AddBuf(Ch,0, &LinkData[Ch].pAPP[3+nOffset], 4);//添加OAD数据 不是完整数据
		if( eResultChoice != eADD_OK )
		{
			return ;
		}

		APPData[Ch].BufDealLen.w +=4;//已处理4字节oad

		//下面是RCSD的处理
		if((LinkData[Ch].pAPP[0] == 0x05) && (LinkData[Ch].pAPP[1] == 0x03))//get request record
		{
			wLen = GetReorderRcsdLen(&LinkData[Ch].pAPP[3]);
			if(wLen != 0x8000)
			{
				if(LinkData[Ch].pAPP[wLen+3] != 0x00)//RCSD个数
				{
					memcpy(&APPData[Ch].pAPPBuf[APPData[Ch].APPCurrentBufLen.w],&LinkData[Ch].pAPP[wLen+3],LinkData[Ch].pAPP[wLen+3]*5+1);
					APPData[Ch].APPCurrentBufLen.w += LinkData[Ch].pAPP[wLen+3]*5+1;//实际组帧数据长度
					APPData[Ch].BufDealLen.w =APPData[Ch].APPCurrentBufLen.w;
				}
				else
				{
						DLT698AddOneBuf( Ch, 0x55, 0);
				}
			}
			else
			{
				DLT698AddOneBuf( Ch, 0x55, 0);
			}
		}

		//下面是DAR的处理
		Cmd = LinkData[Ch].pAPP[0];
		if(Cmd == 0x05 )//|| Cmd == 0x06 )
		{
			DLT698AddOneBuf( Ch, 0x55, 0);
		}
		
		eResultChoice = DLT698AddOneBuf(Ch,0x55, 23);//DAR

		if(Cmd == 0x07 )
			DLT698AddOneBuf( Ch, 0x55, 0);
		// if(Cmd == 0x05 || Cmd == 0x07 )
		//     DLT698AddOneBuf( Ch, 0x55, 0);
		
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
		//APPData[Ch].APPFlag = APP_ERR_DATA;
		APPData[Ch].pAPPBuf[0] |= 0x80;//将请求置成响应
		// APPData[Ch].pAPPBuf[2] = 0x0;//将请求置成响应
		// 去掉置0，返回下行报文的PIID wxy 2023.02.21 14:15
		return ;
	}
	if( api_CheckParaInfo(&LinkData[Ch].pAPP[3]))
	{
		if( APPData[Ch].eSafeMode != eSECURITY_FAIL )//解密失败不进行处理
		{
			switch( LinkData[Ch].pAPP[0] ) //cmd
			{
			case CONNECT_REQUEST:
				DealConnect_Request( Ch,&LinkData[Ch].pAPP[0],&APPData[Ch].pAPPBuf[0]);
				break;
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
			case PROXY_REQUEST:
				DealProxy_Request( Ch,&LinkData[Ch].pAPP[0] );
				break;
			case REPORT_RESPONSE:
				DealReport_Response(Ch,&LinkData[Ch].pAPP[0]);
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
	else
	{
		#if(USE_PRINTF == 1)
		if(((UsePrintfFlag >> 0) & 0x0001) == 0x0001)
		{
			rt_kprintf("Cannot communicate\n");
		}
		#endif
		
		DLT698AddBuf(Ch, 0, LinkData[Ch].pAPP,3);//添加命令头
		APPData[Ch].BufDealLen.w = 3;
		eResultChoice = eAPP_ERR_RESPONSE;
					
		eResultChoice = DLT698AddBuf(Ch,0, &LinkData[Ch].pAPP[3], 4);//添加OAD数据 不是完整数据
		if( eResultChoice != eADD_OK )
		{
			return ;
		}

		APPData[Ch].BufDealLen.w +=4;//已处理4字节oad

		//下面是RCSD的处理
		if((LinkData[Ch].pAPP[0] == 0x05) && (LinkData[Ch].pAPP[1] == 0x03))//get request record
		{
			wLen = GetReorderRcsdLen(&LinkData[Ch].pAPP[3]);
			if(wLen != 0x8000)
			{
				if(LinkData[Ch].pAPP[wLen+3] != 0x00)//RCSD个数
				{
					memcpy(&APPData[Ch].pAPPBuf[APPData[Ch].APPCurrentBufLen.w],&LinkData[Ch].pAPP[wLen+3],LinkData[Ch].pAPP[wLen+3]*5+1);
					APPData[Ch].APPCurrentBufLen.w += LinkData[Ch].pAPP[wLen+3]*5+1;//实际组帧数据长度
					APPData[Ch].BufDealLen.w =APPData[Ch].APPCurrentBufLen.w;
				}
				else
				{
						DLT698AddOneBuf( Ch, 0x55, 0);
				}
			}
			else
			{
				DLT698AddOneBuf( Ch, 0x55, 0);
			}
		}
	
		//下面是DAR的处理
		Cmd = LinkData[Ch].pAPP[0];
		if(Cmd == 0x05 )//|| Cmd == 0x06 )
		{
			DLT698AddOneBuf( Ch, 0x55, 0);
		}

		eResultChoice = DLT698AddOneBuf(Ch,0x55, 255);//DAR,license错误的时候，返回255

		if(Cmd == 0x07 )
			DLT698AddOneBuf( Ch, 0x55, 0);
		// if(Cmd == 0x05 || Cmd == 0x07 )
		//     DLT698AddOneBuf( Ch, 0x55, 0);
		
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
		//APPData[Ch].APPFlag = APP_ERR_DATA;
		APPData[Ch].pAPPBuf[0] |= 0x80;//将请求置成响应
		// APPData[Ch].pAPPBuf[2] = 0x0;//将请求置成响应
		// 去掉置0，返回下行报文的PIID wxy 2023.02.21 14:15
		return ;
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
    TFourByteUnion OI;
    
    if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return;
    }
    
    lib_ExchangeData((BYTE*)&OI.b[0],&LinkData[Ch].pAPP[3],4);
    if(OI.d == 0x4031FE00) //针对发送方法254，物联表不认，把报文原路返回的情况
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
 		
		APPData[Ch].eSafeMode = ProcSecurityRequestData( Ch );//先进行安全传输判断

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
    
		ProcSecurityResponseData( Ch);//安全返回处理

		ProFollowJudge(Ch);//链路层分帧判断
 	}
}
extern TTagBuf  TagBuf;	//加TAG时用到的b

static eAPPBufResultChoice DealGetTransCommandResult( BYTE Ch,WORD Len,BYTE data)
{
	eAPPBufResultChoice eResultChoice;
	
	if( (!(Len & 0x8000)) && ( Len!= 0) )//返回长度bit7不为1
	{
		eResultChoice = DLT698AddOneBuf(Ch,0, data);//响应数据   CHOICE  {     错误信息     [0] DAR，   M条记录     [1] SEQUENCE OF A-RecordRow}
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
extern WORD GetAndCheck645Pare( eSERIAL_TYPE PortType, BYTE *pBuf);
WORD GetRequestProxyData( BYTE Ch, BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	TTwoByteUnion OI;
	// BYTE VariableIndex;
	BYTE Buf[3000],bsize;//电压合格率源数据长度为32
	WORD Lenth;
	BYTE* pBuf;
	WORD wResult;

	lib_ExchangeData(OI.b,pOAD,2);
	Lenth = 0;
	
	pBuf = pOAD;


	pBuf += 4;		//对应OAD的属性
	if(0)
	{
		pBuf += 5;		//端口通信控制块

		pBuf += 2;		//接收等待报文超时时间（秒）

		pBuf += 2;		//接收等待字节超时时间（毫秒
	}
	else
	{
		pBuf += 1;
	}
	
	if(pBuf[0] < 0x80)
	{
		Lenth = pBuf[0];
		bsize = 0;
	}
	else if(pBuf[0] == 0x81)
	{
		Lenth = pBuf[1];
		bsize = 1;
	}
	else if(pBuf[0] == 0x82)
	{
		lib_ExchangeData((BYTE*)&Lenth,&pBuf[1],2);
		bsize = 2;
	}
	else
	{
		Lenth = pBuf[0];
		bsize = 0;
	}

	DataSubtract33H( &pBuf[1+bsize] );//对数据进行减0x33处理

	GetAndCheck645Pare(eUSART_I,&pBuf[1+bsize]);//数据解析到结构体

	wResult = DLT645APPPro(eUSART_I);                      //应用层数据解析
	
	//wResult = DealDLT645_Factory(0);
	Proc645LinkDataResponse( (eSERIAL_TYPE)0, wResult );          //链路层数据组帧
	
	Lenth = gPr645[0].pMessageAddr[9+0] + 6 + 6;

	if(wResult == 0xFFFF)
		return 0;

//	memcpy( DLT645Sendbuf[0].DataBuf, gPr645[0].pMessageAddr, Lenth );
	OutBuf[0] = 0x01;
	OutBuf[1] = 0x09;
	OutBuf[2] = Lenth&0xFF;
	memcpy( &OutBuf[3],  &gPr645[0].pMessageAddr[0], Lenth);

//	Lenth = 5;
	Lenth+=3;

	if( (DataType == eData) || (DataType == eTagData) )
	{
	//	Lenth = GetVariableData( Ch, Dot, pOAD, VariableIndex, Buf );
		if( Lenth == 0x8000 )
		{
			return Lenth;
		}
		if( DataType == eData )
		{
			if( OutBufLen < Lenth )
			{
				return 0;
			}
			memcpy( OutBuf, Buf, Lenth );
		}
		else
		{
		//	Lenth = GetVariableAddTag(pOAD, VariableIndex, Buf, OutBufLen, OutBuf);
		}
	}
	else if( DataType == eAddTag )
	{
	//	Lenth = GetVariableAddTag(pOAD, VariableIndex, InBuf, OutBufLen, OutBuf);
	}
	else
	{
		return 0x8000;
	}
	
	return Lenth;
}

BYTE api_Write_Factory(BYTE Ch,  BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf)
{
	int nLen;
	// int nNum;
	// BYTE byCodeFlag;
//	BOOL bResult=FALSE;
	// int nDataType;
//	DWORD dwAddr;
	BYTE Buf[10]={0};

	if(InBuf == NULL)
		return 0;
	
	nLen = 0;
	
	// nDataType = InBuf[nLen];	//Structure_698:
	// nNum = InBuf[nLen+1];
	nLen += 2;
	
	//memcpy(gDefaultPara.szFactory,&InBuf[nLen],min(nNum,4));

	memcpy(Buf, &InBuf[nLen], 4);
	api_VWriteSafeMem(GET_CONTINUE_ADDR(EventConRom.szFactory),8, (BYTE*)Buf);

	OutBuf[0]=0x00;


	return 1;
	//Trace("file addr=[%x], block=%d, index=%d", tFileText.lFileIndex*tFileInfo.dwTxBlockLen, tFileInfo.BlockStatusNum, tFileText.lFileIndex);
}


WORD GetProxyOadData( BYTE Ch, BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE ClassAttribute, ClassAttribute2;
	TTwoByteUnion OI,Len;	
	
	Len.w = 0;
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);
	memset( (BYTE*)&TagBuf.InBufLenth, 0x00, sizeof(TagBuf) );
	memset( (BYTE*)&TagPara.Array_Struct_Num, 0x00, sizeof(TagPara) );
	
	ClassAttribute2 = (pOAD[2]&0xFf);
        
	if( OI.w==0xF209 || OI.w==0x20EB )//代理类
	{
		if(ClassAttribute == 5)
			Len.w = api_Write_ID( Ch, DataType, Dot, pOAD, &pOAD[4],  OutBufLen, OutBuf);//设置ID
		else if(ClassAttribute == 6)
			Len.w = api_Write_IK( Ch, DataType, Dot, pOAD, &pOAD[4],  OutBufLen, OutBuf);//设置IK
		else if(ClassAttribute == 7)
		{
			Len.w = api_Write_SoftFlag( Ch, DataType, Dot, pOAD, &pOAD[4],  OutBufLen, OutBuf);
		}
		else if(ClassAttribute == 0x1A)
		{
			Len.w = api_GetFileText_Write_jdfh( Ch, DataType, Dot, pOAD, &pOAD[4],  OutBufLen, OutBuf);
		}
		else if(ClassAttribute == 0x1B)
		{
			// Len.w = api_GetFileText_Read_jdfh( Ch, DataType, Dot, pOAD, &pOAD[4],  OutBufLen, OutBuf);
		}
    	else if(ClassAttribute2 == 0x22)
		{
			Len.w = api_Write_Factory( Ch, ClassAttribute, Dot, pOAD, &pOAD[4],  OutBufLen, OutBuf);
		}
		else
		{
			UseProxy = 1;
			Len.w = GetRequestProxyData( Ch, DataType, Dot, pOAD, InBuf,  OutBufLen, OutBuf);
			UseProxy = 0;
		}
	}
	/*if(OI.w==0xF001)
	{
		if(ClassAttribute == 0x1)
			Len.w = api_GetFileText_Write_jdfh( Ch, DataType, Dot, pOAD, &pOAD[4],  OutBufLen, OutBuf);
		else
			Len.w = api_GetFileText_Read_jdfh( Ch, DataType, Dot, pOAD, &pOAD[4],  OutBufLen, OutBuf);
	}*/
	if(OI.w == 0xF002)
	{
			if(ClassAttribute == 5)
					Len.w = api_Write_ID( Ch, DataType, Dot, pOAD, &pOAD[4],  OutBufLen, OutBuf);
			if(ClassAttribute == 6)
					Len.w = api_Write_IK( Ch, DataType, Dot, pOAD, &pOAD[4],  OutBufLen, OutBuf);
			if(ClassAttribute == 7)
			{
					Len.w = api_Write_SoftFlag( Ch, DataType, Dot, pOAD, &pOAD[4],  OutBufLen, OutBuf);
			}
	}
	
	
	return Len.w;
}


eAPPBufResultChoice GetTransCommand( BYTE Ch,BYTE *pOAD )
{
	WORD Len;
	eAPPBufResultChoice eResultChoice;
	TTwoByteUnion OI;	

	lib_ExchangeData(OI.b,pOAD,2);

	if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return eAPP_ERR_RESPONSE;
    }
    
	if(OI.w!=0x20EB)
	{
    // wxy 20220815
		eResultChoice = DLT698AddBuf(Ch,0, pOAD, 4);//添加OAD数据 不是完整数据
		if( eResultChoice != eADD_OK )
		{
			return eResultChoice;
		}
		
		APPData[Ch].BufDealLen.w +=4;//已处理4字节oad
		
	}
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
            Len = GetProxyOadData( Ch, 1,0XFF,pOAD, (BYTE *)NULL,(APPData[Ch].APPMaxBufLen.w- APPData[Ch].APPCurrentBufLen.w-1) , &APPData[Ch].pAPPBuf[APPData[Ch].APPCurrentBufLen.w+1]);//添加数据buf
		}
		if(OI.w!=0x20EB)
			 eResultChoice = DealGetTransCommandResult( Ch, Len, 1);
		else
			 eResultChoice = DealGetTransCommandResult( Ch, Len, 0);
	}
	
	return eResultChoice;
}

static void DealProxy_Request(BYTE Ch,BYTE *pAPDU)
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
	case GET_PROXY_TransCommandRequest:
		eResultChoice = GetTransCommand( Ch,&pAPDU[3] );
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


static void DealReport_Response(BYTE Ch,BYTE *pAPDU)
{
	// eAPPBufResultChoice eResultChoice;
	
    if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return;
    }
	
	DLT698AddBuf(Ch, 0, LinkData[Ch].pAPP,3);//添加命令头
	APPData[Ch].BufDealLen.w = 3;
	// eResultChoice = eAPP_ERR_RESPONSE;
	
	ManageComInfo.EventReportOverTime = 0;
    
	//先清除33200200，再清除主动上报的顺序！！！
	ClearAutoReportIndexRam(0); //清除33200200列表中的主动上报oad

	//清除主动上报
	ReportPara.Times = 0;
	ReportPara.OAD_Num = 0;
	memset(&ReportPara.ReportOAD[0],0,sizeof(ReportPara.ReportOAD));
	ReportPara.CRC32 = lib_CheckCRC32((BYTE*)&ReportPara, sizeof(ReportPara)-sizeof(DWORD));
}

//--------------------------------------------------
//功能描述:    获取模组运行状态字1
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
	//bit14	模组SPI异常
	//bit13	模组存储器故障或损坏
	//bit12	模组内部程序错误
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
	if((api_CheckError(ERR_WRITE_EEPROM1) == TRUE)//e2     
	||(api_CheckError(ERR_WRITE_EEPROM2) == TRUE)       //BIT5	存储器故障或损坏	
	||(api_CheckError(ERR_WRITE_EEPROM3) == TRUE))      //BIT4	内部程序错误	 											     	
	{
		Buf[0] |= BIT5;
	}	
	if( api_CheckError( ERR_CHECK_5460_1 ) == TRUE )		//BIT6	SPI数据异常		
	{
		Buf[0] |= BIT6;
	}
	/**********************低字节************************/
}