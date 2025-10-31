//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部
//创建人	姜文浩
//创建日期	2016.12.27
//描述		DL/T 698.45面向对象协议电能读取C文件
//修改记录
//----------------------------------------------------------------------
#include "AllHead.h"
#include "Dlt698_45.h"
#if ( SEL_DLT698_2016_FUNC == YES)
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
// 索引信息在EEPROM中的地址范围

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
extern T_ApplFrzData tApplFrzDataFLASH[NILM_EQUIPMENT_MAX_NO];
//-----------------------------------------------
//				本文件使用的变量，常量

#define MAX_PARA_BUF_LENTH         (MAX_TIME_SEG*3*MAX_TIME_SEG_TABLE+30)//时段表源数据长度为最大数据长度

static const BYTE T_MinI[]			= { Visible_string_698,0XFF };//最小电流和转折电流	

static const BYTE T_MeterNO[]		= { Octet_string_698, 0x06 };		//有符号需量数据标识
static const BYTE T_LCDPara[]		= //LCD参数		
{ 
	Structure_698, 	 	7, 
	Unsigned_698, 		1, 
	Long_unsigned_698, 	2, 
	Long_unsigned_698, 	2, 
	Long_unsigned_698, 	2, 
	Unsigned_698,		1, 
	Unsigned_698,		1, 
	Unsigned_698,		1 
};

static const BYTE T_TimezoneNum[]	= //时区时段数
{ 
	Structure_698, 	 	5, 
	Unsigned_698, 		1,  
	Unsigned_698,		1, 
	Unsigned_698,		1, 
	Unsigned_698,		1,
	Unsigned_698,		1 
};

static const BYTE T_TimeHolidayTable[]	= //公共假日数
{ 
	Array_698,		0XFF,
	Structure_698,	2,
	Date_698,		5,
	Unsigned_698,	1
};

static const BYTE T_Timezone[]			= //时区表
{ 
	Array_698,		0XFF,
	Structure_698,	03,
	Unsigned_698,	1,
	Unsigned_698,	1,
	Unsigned_698,	1
};

static const BYTE T_VPCT[]				= //电压合格率
{ 
	Structure_698,		4,
	Long_unsigned_698,	2,
	Long_unsigned_698,	2,
	Long_unsigned_698,	2,
	Long_unsigned_698,	2,
};

static const BYTE T_BillingPara[]		= //结算日
{ 
	Array_698,		0xff,
	Structure_698,	2,
	Unsigned_698,	1,
	Unsigned_698,	1,
};

static const BYTE T_ModeInfo[]		= //版本信息
{ 
	Structure_698,		6,
	Visible_string_698,	4,//厂商代码
	Visible_string_698,	4,//软件版本号
	Visible_string_698,	6,//软件版本日期
	Visible_string_698,	4,//硬件版本号
	Visible_string_698,	6,//硬件版本日期
	Visible_string_698,	8,//厂家扩展信息
};

static const BYTE T_ResportChanel[]		= //上报通道
{
	Array_698,			0xff,
	OAD_698,			4,
};

static const BYTE T_PositionInfo1[]		= //地理位置信息1
{
	Structure_698, 	4,
	Enum_698,		1,
	Unsigned_698,	1,
	Unsigned_698,	1,
	Unsigned_698,	1,
};

static const BYTE T_APPInfo[]		= //应用语境信息
{
	Structure_698, 	      7,
	Long_unsigned_698,	  2,
	Long_unsigned_698,	  2,
	Long_unsigned_698,	  2,
	Long_unsigned_698,	  2,
	Bit_string_698,       64,
	Bit_string_698,       128,
	Double_long_unsigned_698, 4
};

static const BYTE T_OI[]     = //支持设备列表
{
    Array_698,   1,
    OI_698,      2,
};

static const BYTE T_ProtoclList[]      = //支持规约列表
{
    Array_698,           1,
    Visible_string_698,  0xff,
};

static const BYTE T_Rate[]		= //费率电价-阶梯电价的阶梯值与阶梯电价数组
{
    Array_698,                  0xff,
    Double_long_unsigned_698,   4,
};

static const BYTE T_LimitMoney[]	= //报警金额限值
{
    Structure_698,             2,
    Double_long_unsigned_698,  4,
    Double_long_unsigned_698,  4,
};

static const BYTE T_OtherLimitMoney[]	= //其他金额限值
{
    Structure_698,             3,
    Double_long_unsigned_698,  4,
    Double_long_unsigned_698,  4,
    Double_long_unsigned_698,  4,
};

static const BYTE T_YearBill[]		= //阶梯结算日
{
    Structure_698,             3,
    Unsigned_698,              1,
    Unsigned_698,              1,
    Unsigned_698,              1,
};

static const BYTE T_TimeBroadCastPara[]		= //广播校时参数
{
    Structure_698,             2,
    Long_unsigned_698,         2,
    Long_unsigned_698,		   2,
};

static const BYTE Array_OI[] = 
{
    0x01, 0x0a, 0x02, 0x02, 0x50, 0x00, 0x00, 0x02, 0x02, 0x01, 
    0x01, 0x02, 0x02, 0x11, 0x80, 0x16, 0x00, 0x01, 0x01, 0x02, 
    0x02, 0x11, 0x05, 0x03, 0x00, 0x02, 0x02, 0x50, 0x00, 0x80, 
    0x02, 0x02, 0x01, 0x01, 0x02, 0x02, 0x11, 0x05, 0x16, 0x00, 
    0x01, 0x01, 0x02, 0x02, 0x11, 0x00, 0x03, 0x80, 0x02, 0x02, 
    0x50, 0x00, 0x05, 0x02, 0x02, 0x01, 0x01, 0x02, 0x02, 0x11, 
    0x00, 0x16, 0x80, 0x01, 0x01, 0x02, 0x02, 0x11, 0x00, 0x03, 
    0x05, 0x02, 0x02, 0x50, 0x80, 0x00, 0x02, 0x02, 0x01, 0x01, 
    0x02, 0x02, 0x11, 0x00, 0x16, 0x05, 0x01, 0x01, 0x02, 0x02, 
    0x11, 0x00, 0x03, 0x00, 0x02, 0x02, 0x50, 0x05, 0x00, 0x02, 
    0x02, 0x01, 0x01, 0x02, 0x02, 0x11, 0x00, 0x16, 0x00, 0x01, 
    0x01, 0x02, 0x02, 0x11, 0x80, 0x03, 0x00, 0x02, 0x02, 0x50, 
    0x00, 0x00, 0x02, 0x02, 0x01, 0x01, 0x02, 0x02, 0x11, 0x80, 
    0x16, 0x00, 0x01, 0x01, 0x02, 0x02, 0x11, 0x05, 0x03, 0x00, 
    0x02, 0x02, 0x50, 0x00, 0x80, 0x02, 0x02, 0x01, 0x01, 0x02, 
    0x02, 0x11, 0x05, 0x16, 0x00, 0x01, 0x01, 0x02, 0x02, 0x11, 
    0x00, 0x03, 0x80, 0x02, 0x02, 0x50, 0x00, 0x00, 0x02, 0x02, 
    0x01, 0x01, 0x02, 0x02, 0x11, 0x6d, 0x16, 0x9a, 0x01, 0x01, 
    0x02, 0x02, 0x11, 0x16, 0x03, 0x03, 0x02, 0x02, 0x50, 0x11, 
    0x0b, 0x02, 0x02, 0x01, 0x01, 0x02, 0x02, 0x11, 0x00, 0x16, 
    0x11, 0x01, 0x01, 0x02, 0x02, 0x11, 0x04, 0x03, 0x01, 0x02, 
    0x02, 0x50, 0x03, 0x02, 0x02, 0x02, 0x01, 0x01, 0x02, 0x02, 
    0x11, 0x11, 0x16, 0x08, 0x01, 0x01, 0x02, 0x02, 0x11, 0x11, 
    0x03, 0x00,
};

//0x4800 属性2定义，用电设备类号
static const BYTE A_4800_2_ApplClsID[] =
{
	Array_698,			0xff,
	Long_unsigned_698,	2,	//设备类号
};

//0x4800 属性3定义，电器设备当前数据集
static const BYTE A_4800_3_ApplCurData[] =
{
	Array_698,			0xff,
	Structure_698, 	 	3,
	Long_unsigned_698,	2,	//设备类号
	Enum_698,			1,	//设备状态
	Double_long_698,	4,	//设备有功功率
};
//0x4800 属性4定义，电器设备冻结数据单元
const BYTE A_4800_4_ApplFrzData[] =
{
	Array_698,					0xff,
	Structure_698, 	 			6,
	Long_unsigned_698,			2,	//设备类号
	Double_long_unsigned_698,	4,	//冻结周期内有功用电量
	Double_long_698,			4,	//冻结周期内设备运行平均功率
	Unsigned_698,				1,	//启动时间
	Unsigned_698,				1,	//停止时间
	Unsigned_698,				1,	//合并启停数
};
//0x4800 属性4定义，电器设备冻结，非电热类
const BYTE A_4800_4_ApplFrzData_NotElecHot[] =
{
	Array_698,					0xff,
	Structure_698, 	 			6,
	Long_unsigned_698,			2,	//设备类号
	Double_long_unsigned_698,	4,	//冻结周期内有功用电量
	Double_long_698,			4,	//冻结周期内设备运行平均功率
	NULL_698,					1,	//启动时间
	NULL_698,					1,	//停止时间
	NULL_698,					1,	//合并启停数
};
//增加数据类型请查看源数据和加TAG是否支持
static const TCommonObj ParaMeterObj[] =
{
	{  0x4000,  &SU_00,   	T_DateTimS		},//日期时间---增加广播校时参数 格式为T_TimeBroadCastPara
	{  0x4001,  &SU_00,   	T_MeterNO		},//通信地址
	{  0x4002,  &SU_00,   	T_MeterNO		},//表号
	{  0x4003,  &SU_00,   	T_MeterNO		},//客户编号
	{  0x4004,  &SU_00,   	T_Special		},//设备地理位置 特殊处理
	{  0x4007,  &SU_00,   	T_LCDPara		},//LCD参数
	{  0x4008,  &SU_00,   	T_DateTimS		},//两套时区表切换时间
	{  0x4009,  &SU_00,   	T_DateTimS		},//两套时段表切换时间
	{  0x400C,  &SU_00,   	T_TimezoneNum	},//时区时段数
    #if( PREPAY_MODE == PREPAY_LOCAL)
    {  0x400A,  &SU_00,   	T_DateTimS	    },//备用套分时费率切换时间
    {  0x400B,  &SU_00,   	T_DateTimS	    },//备用套阶梯电价切换时间
	{  0x400d,  &SU_00,   	T_Unsigned	    },//阶梯数
	#endif	
	{  0x400F,  &SU_00,   	T_Unsigned		},//密钥条数
	{  0x4010,  &SU_00,   	T_Unsigned		},//计量元件数
	{  0x4011,  &SU_00,   	T_TimeHolidayTable},//公共假日数
	{  0x4012,  &SU_00,   	T_BitString8	},//周休日特征字
	{  0x4013,  &SU_00,   	T_Unsigned		},//周休日采用的时段表号
	{  0x4014,  &SU_00,   	T_Timezone		},//当前套时区表
	{  0x4015,  &SU_00,   	T_Timezone		},//备用套时区表
	{  0x4016,  &SU_00,   	T_Special		},//当前套时段表 特殊处理
	{  0x4017,  &SU_00,   	T_Special		},//备用套时段表 特殊处理
	{  0x401C,  &SU_2,   	T_UNDoubleLong	},//电流互感器变比
	{  0x401D,  &SU_2,   	T_UNDoubleLong	},//电压互感器变比
	#if( PREPAY_MODE == PREPAY_LOCAL)
    {  0x4018,  &SU_00,     T_Rate          },//当前套费率电价
    {  0x4019,  &SU_00,     T_Rate          },//备用套费率电价
    {  0x401A,  &SU_00,     T_Special       },//当前套阶梯电价
    {  0x401B,  &SU_00,     T_Special       },//备用套阶梯电价,属于参变量类(class_id=8)，没有属性3:换算及单位,中间不能 SU_2Yuan
    {  0x401e,  &SU_00,		T_LimitMoney    },//报警金额限值,属于参变量类(class_id=8)，没有属性3:换算及单位
    {  0x401F,  &SU_00,  	T_OtherLimitMoney},//其他金额限值        
    {  0x4022,  &SU_00,  	T_BitString16	},//插卡状态字
    #endif
	{  0x4102,  &SU_0MS,   	T_Unsigned		},//校表脉冲长度
	{  0x4103,  &SU_00,   	T_VisString32	},//资产管理编码
	{  0x4104,  &SU_00,   	T_VisString6	},//额定电压
	{  0x4105,  &SU_00,   	T_VisString6	},//额定电流/基本电流
	{  0x4106,  &SU_00,   	T_VisString6	},//最大电流
	{  0x4107,  &SU_00,   	T_VisString4	},//有功准确度等级
	{  0x4109,  &SU_01Kwh, 	T_UNDoubleLong	},//电能表有功常数
	{  0x410B,  &SU_0MS,   	T_VisString32	},//电能表型号
	{  0x4111,  &SU_00,  	T_VisString16	},//软件备案号
	{  0x4112,  &SU_00,   	T_BitString8	},//有功组合方式字
	{  0x4116,  &SU_00,   	T_BillingPara	},//结算日
	{  0x4300,  &SU_00,   	T_Special		},//电气设备
    {  0x4400,  &SU_00,   	T_Special		},//应用连接
#if( MAX_PHASE != 1 )
	{  0x4030,  &SU_00,   	T_VPCT			},//电压合格率
	{  0x4100,  &SU_0MIN,   T_Unsigned		},//最大需量周期
	{  0x4101,  &SU_0MIN,   T_Unsigned		},//滑差时间
	{  0x4108,  &SU_00,   	T_VisString4	},//无功准确度等级
	{  0x410A,  &SU_01Kwh,  T_UNDoubleLong	},//电能表无功常数
	{  0x4113,  &SU_00,  	T_BitString8	},//无功组合方式字1
	{  0x4114,  &SU_00,   	T_BitString8	},//无功组合方式字2
	{  0x4117,  &SU_00,   	T_TI			},//期间需量冻结周期
#endif
	{  0x4800,  &SU_00,   	A_4800_4_ApplFrzData_NotElecHot		},//识别结果
};	

//默认生产日期2019-1-1 12：00：00
 const BYTE ProduceDate[7]={227,7,1,1,12,0,0};
//-----------------------------------------------
static BYTE NumofNILM;
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------


//-----------------------------------------------
//				函数定义
//-----------------------------------------------
//--------------------------------------------------
// 功能描述:  读分钟冻结数据时，由于存电器个数不确定，需要读取时进行计算
//
// 参数:      [in]：Num
//
// 返回值:    无
//
// 备注内容:  无
//--------------------------------------------------
void api_SetTagArrayNum(BYTE Num)
{
	NumofNILM = Num;
}
//--------------------------------------------------
//功能描述:  根据表格查找参变量OAD
//         
//参数:      OI[in]：OI
//         
//返回值:    参变量索引
//         
//备注内容:  无
//--------------------------------------------------
static BYTE SearchParaMeterOAD( WORD OI )
{
	BYTE i,Num;//数组个数不能超过255

	Num = (sizeof(ParaMeterObj)/sizeof(TCommonObj));
	if( Num >= 0x80 )//避免死循环
	{
		return 0x80;
	}
	
	for (i=0; i<Num; i++)
	{
		if( OI == ParaMeterObj[i].OI )
		{
			return i;
		}
	}

	if( i == Num )//未找到
	{
		return 0x80;
	}
	return 0x80;
}
//--------------------------------------------------
//功能描述:     获取设备管理（class19）属性数据（不带Tag）//电气设备类
//         
//参数:     
//          *pOAD[in]：	OAD             
//          *pBuf[out]：输出数据
//         
//返回值:   数据长度
//         
//备注内容: 处理除属性2外的属性 不含Tag
//--------------------------------------------------
static WORD GetMeterClass19Data( BYTE *pOAD, BYTE *pBuf )
{
	BYTE ClassAttribute,ClassIndex;
	WORD Len,Result;
	const BYTE *Tag;
	
	ClassAttribute = (pOAD[2]&0x1f);//bit0…bit4编码表示对象属性编号；
	ClassIndex = pOAD[3];
	Len = 0x8000;
	
	switch( ClassAttribute )
	{
		case 2://设备描述符
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}

			Len = 32;
			//电表型号
			Result = api_ProcMeterTypePara( READ, eMeterMeterModel, pBuf );
			if(Result == FALSE)
			{
				Len = 0x8000;
				break;
			}
			break;

		case 3://版本信息
			if( ClassIndex > T_ModeInfo[1] )
			{
				return 0x8000;
			}

			Tag = T_ModeInfo;
			Tag += GetTagOffsetAddr( ClassIndex, Tag );
			Len = GetTypeLen( eData, Tag );
			//厂商代码
			if( (ClassIndex==0) || (ClassIndex==1) )
			{
				Result = api_ReadFromContinueEEPRom(GET_CONTINUE_ADDR( ParaConRom.MeterTypePara.FactoryCode[0] ), 4, pBuf);
				if(Result == FALSE)
				{
					return 0x8000;
				}
				pBuf += 4;
			}
			//软件版本号
			if( (ClassIndex ==0) || (ClassIndex == 2))
			{
				Result = api_ReadFromContinueEEPRom(GET_CONTINUE_ADDR( ParaConRom.MeterTypePara.SoftWareVersion[0] ), 4, pBuf);
				if(Result == FALSE)
				{
					return 0x8000;
				}

				pBuf += 4;
			}

			//软件版本日期 ASCII 码
			if( (ClassIndex==0) || (ClassIndex==3) )
			{
			    Result = api_ReadFromContinueEEPRom(GET_CONTINUE_ADDR( ParaConRom.MeterTypePara.SoftWareDate[0] ), 6, pBuf);
                if(Result == FALSE)
				{
					return 0x8000;
				}
				pBuf += 6;
			}

			//硬件版本号
			if( (ClassIndex==0) || (ClassIndex==4) )
			{
				Result = api_ReadFromContinueEEPRom(GET_CONTINUE_ADDR( ParaConRom.MeterTypePara.HardWareVersion[0] ), 4, pBuf);
				if(Result == FALSE)
				{
					return 0x8000;
				}

				pBuf += 4;
			}

			//硬件版本日期 ASCII码
			if( (ClassIndex==0) || (ClassIndex==5) )
			{
				Result = api_ReadFromContinueEEPRom(GET_CONTINUE_ADDR( ParaConRom.MeterTypePara.HardWareDate[0] ), 6, pBuf);
    			if(Result == FALSE)
				{
					return 0x8000;
				}
				pBuf += 6;
			}
			
			//厂家扩展信息
			if( (ClassIndex==0) || (ClassIndex==6) )
			{
				memset( pBuf,0x00,8);
			}			
			break;

		case 4://生产日期
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			Result = api_ReadFromContinueEEPRom(GET_CONTINUE_ADDR( ParaConRom.MeterTypePara.ProduceDate[0] ), 7, pBuf);
            if( api_CheckClock((TRealTimer *)pBuf)!=TRUE )
            {
                memcpy(pBuf,ProduceDate,7);
            }            
			Len = 7;
			break;

        case 5://子设备列表
			if( ClassIndex > T_OI[1] )
			{
				return 0x8000;
			}

			Tag = T_OI;
			Tag += GetTagOffsetAddr( ClassIndex, Tag );
			Len = GetTypeLen( eData, Tag );
            pBuf[0] = 10;
            pBuf[1] = 18;

            break;

        case 6://支持规约列表
            if( ClassIndex > T_ProtoclList[1] )
			{
				return 0x8000;
			}

			Tag = T_ProtoclList;
			TagPara.Lenth.w = ProtocolVersionConst_698[0];
			Tag += GetTagOffsetAddr( ClassIndex, Tag );
			Len = GetTypeLen( eData, Tag );
			memcpy( pBuf, ProtocolVersionConst_698+1, ProtocolVersionConst_698[0] );
            break;     
		case 7://允许跟随上报
		case 8://允许主动上报
		case 9://允许与主站通话
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}

			if( ClassAttribute == 7 )
			{
				pBuf[0] = ReadReportFlag(eFollowReport);
			}
			else if( ClassAttribute == 8 )
			{
				pBuf[0] = ReadReportFlag(eActiveReport);
			}	
			else
			{
				pBuf[0] = 1;
			}			
			Len = 1;	
			break;

		case 10://上报通道
			Result = ReadReportChannel( pBuf );
			if( Result == FALSE )
			{
				return 0;
			}
			
			if(ClassIndex > (Result/4))
			{
				return 0x8000;
			}
			
			if( ClassIndex != 0 )
			{
				memcpy( pBuf, (BYTE*)&pBuf[4*(ClassIndex-1)],4 );
			}
			TagPara.Array_Struct_Num = (Result/4); 
			Tag = T_ResportChanel;
			Tag += GetTagOffsetAddr( ClassIndex, Tag );
			Len = GetTypeLen( eData, Tag );			
			break;

		default:
			return 0x8000;
	}

	return Len;
}

//--------------------------------------------------
//功能描述:  设备管理（class19）属性数据添加Tag
//         
//参数:      
//         	*pOAD[in]：		OAD     
//          *InBuf[in]：	需要添加tag的数据         
//          OutBufLen[in]：	申请的缓冲长度         
//          *OutBuf[out]：	输出数据
//         
//返回值:   添加tag后的数据长度
//         
//备注内容: 处理除属性2外的属性  
//--------------------------------------------------
static WORD AddTagMeterClass19( BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE ClassAttribute,ClassIndex,Buf[30];
	WORD Len,Result;
	const BYTE *Tag;
	
	ClassAttribute = (pOAD[2]&0x1f);//bit0…bit4编码表示对象属性编号；
	ClassIndex = pOAD[3];
	Len = 0x8000;
	
	switch( ClassAttribute )
	{
		case 2://设备描述符
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			Len = GetTypeLen( eTagData, T_VisString32 );
			if( Len == 0x8000 )
			{
                return 0x8000;
			}
			
			if( OutBufLen < Len )
			{
				return 0;
			}
			Tag = T_VisString32;
			Result = GetRequestAddTag( Tag, InBuf, OutBuf );
			if( Result == 0x8000 )
			{
				return 0x8000;
			}
			break;

		case 3://版本信息
			if( ClassIndex > T_ModeInfo[1] )
			{
				return 0x8000;
			}
			
			Tag = T_ModeInfo;
			Tag += GetTagOffsetAddr( ClassIndex, Tag );
			Len = GetTypeLen( eTagData, Tag );
			if( Len == 0x8000 )
			{
                return 0x8000;
			}
			
			if( OutBufLen < Len )
			{
				return 0;
			}
			Result = GetRequestAddTag( Tag, InBuf, OutBuf );
			if( Result == 0x8000 )
			{
				return 0x8000;
			}
			break;

		case 4://生产日期
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			Tag = T_DateTimS;
			Len = GetTypeLen( eTagData, Tag );
			if( Len == 0x8000 )
			{
                return 0x8000;
			}
			
			if( OutBufLen < Len )
			{
				return 0;
			}
			Result = GetRequestAddTag( Tag, InBuf, OutBuf );
			if( Result == 0x8000 )
			{
				return 0x8000;
			}
			break;
			
		case 5://子设备列表
			if( ClassIndex > T_OI[1] )
			{
				return 0x8000;
			}

			Tag = T_OI;
			Tag += GetTagOffsetAddr( ClassIndex, Tag );
			Len = GetTypeLen( eTagData, Tag );
			if( Len == 0x8000 )
			{
                return 0x8000;
			}
			
			if( OutBufLen < Len )
			{
				return 0;
			}
			Result = GetRequestAddTag( Tag, InBuf, OutBuf );
			if( Result == 0x8000 )
			{
				return 0x8000;
			}
			break;	
			
        case 6://支持规约列表
            if( ClassIndex > T_ProtoclList[1] )
            {
                return 0x8000;
            }
            
            Tag = T_ProtoclList;
            TagPara.Lenth.w = ProtocolVersionConst_698[0];
            Tag += GetTagOffsetAddr( ClassIndex, Tag );
            Len = GetTypeLen( eTagData, Tag );
            if( Len == 0x8000 )
            {
                return 0x8000;
            }
            
            if( OutBufLen < Len )
            {
                return 0;
            }
            Result = GetRequestAddTag( Tag, InBuf, OutBuf );
            if( Result == 0x8000 )
            {
                return 0x8000;
            }

            break;  

			break;	

		case 7://允许跟随上报
		case 8://允许主动上报
		case 9://允许与主站通话
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			
			Tag = T_Bool;
			Len = GetTypeLen( eTagData, Tag );
		    if( Len == 0x8000 )
			{
                return 0x8000;
			}
			
			if( OutBufLen < Len )
			{
				return 0;
			}
			Result = GetRequestAddTag( Tag, InBuf, OutBuf );
			if( Result == 0x8000 )
			{
				return 0x8000;
			}			
			break;

		case 10://上报通道
			Result = ReadReportChannel( Buf );
			if( Result == FALSE )
			{
				Result = 0;
			}
			
			if(ClassIndex > (Result/4))
			{
				return 0x8000;
			}
			
			TagPara.Array_Struct_Num = (Result/4); 

			Tag = T_ResportChanel;
			Tag += GetTagOffsetAddr( ClassIndex, Tag );
			Len = GetTypeLen( eTagData, Tag );
			if( Len == 0x8000 )
			{
                return 0x8000;
			}
			
			if( OutBufLen < Len )
			{
				return 0;
			}
			Result = GetRequestAddTag( Tag, InBuf, OutBuf );
			if( Result == 0x8000 )
			{
				return 0x8000;
			}
			break;

		default:
			return 0x8000;
	}

	return Len;
}

//--------------------------------------------------
//功能描述:  获取电气设备类（class19）数据长度
//         
//参数:      
//         	DataType[in]：		DataType[in]：eData/eTagData     
//          *pOAD[in]：			OAD          
//         
//返回值:   添加tag后的数据长度
//         
//备注内容:  
//--------------------------------------------------
static WORD GetMeterClass19Len( BYTE DataType, BYTE *pOAD )
{
	BYTE ClassAttribute,ClassIndex;
	WORD Len;
	const BYTE *Tag;

	ClassAttribute = (pOAD[2]&0x1f);//bit0…bit4编码表示对象属性编号；
	ClassIndex = pOAD[3];

	switch( ClassAttribute )
	{
		case 2://设备描述符
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			Len = GetTypeLen( DataType, T_VisString32 );
			break;

		case 3://版本信息
			if( ClassIndex > T_ModeInfo[1] )
			{
				return 0x8000;
			}
			
			Tag = T_ModeInfo;
			Tag += GetTagOffsetAddr( ClassIndex, Tag );
			Len = GetTypeLen( DataType, Tag );
			break;

		case 4://生产日期
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			Tag = T_DateTimS;
			Len = GetTypeLen( DataType, Tag );
			break;

        case 5://子设备列表
            if( ClassIndex > T_OI[1] )
			{
				return 0x8000;
			}
			
			Tag = T_OI;
			Tag += GetTagOffsetAddr( ClassIndex, Tag );
			Len = GetTypeLen( DataType, Tag );
            break;
            
        case 6://支持规约列表
            if( ClassIndex > T_ProtoclList[1] )
			{
				return 0x8000;
			}
			
			Tag = T_ProtoclList;
			TagPara.Lenth.w = ProtocolVersionConst_698[0];
			Tag += GetTagOffsetAddr( ClassIndex, Tag );
			Len = GetTypeLen( DataType, Tag );
            break;
		case 7://允许跟随上报
		case 8://允许主动上报
		case 9://允许与主站通话
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			
			Tag = T_Bool;
			Len = GetTypeLen( DataType, Tag );
			break;
                
		case 10://上报通道
			if(ClassIndex > MAX_COM_CHANNEL_NUM)
			{
				return 0x8000;
			}
			
			TagPara.Array_Struct_Num = 4; //按照最大进行返回

			Tag = T_ResportChanel;
			Tag += GetTagOffsetAddr( ClassIndex, Tag );
			Len = GetTypeLen( DataType, Tag );
			if( Len == 0x8000 )
			{
                return 0x8000;
			}
			break;

		default:
			return 0x8000;
	}
	return Len;
}


//--------------------------------------------------
//功能描述:     获取设备管理（class20）属性数据（不带Tag）//应用连接类
//         
//参数:     
//          *pOAD[in]：	OAD             
//          *pBuf[out]：输出数据
//         
//返回值:   数据长度
//         
//备注内容: 处理除属性2外的属性 不含Tag
//--------------------------------------------------
static WORD GetMeterClass20Data( BYTE *pOAD, BYTE *pBuf )
{
	BYTE ClassAttribute,ClassIndex;
	WORD Len;
	const BYTE *Tag;
	TTwoByteUnion TmpLen;
	
	ClassAttribute = (pOAD[2]&0x1f);//bit0…bit4编码表示对象属性编号；
	ClassIndex = pOAD[3];
	Len = 0x8000;
	
	switch( ClassAttribute )
	{
	    case 2://对象列表
            memcpy( pBuf, Array_OI, sizeof( Array_OI ) );
            return sizeof( Array_OI );
	        break;
		case 3://应用语境信息
			if( ClassIndex > T_APPInfo[1] )
			{
				return 0x8000;
			}

			Tag = T_APPInfo;
			Tag += GetTagOffsetAddr( ClassIndex, Tag );
			Len = GetTypeLen( eData, Tag );
			//协议版本信息
			if( (ClassIndex==0) || (ClassIndex==1) )
			{
                api_ProcMeterTypePara( READ, eMeterProtocolVersion, pBuf );//698.45协议版本号(数据类型:WORD)
				pBuf += 2;
			}
			//最大接受apdu尺寸
			if( (ClassIndex ==0) || (ClassIndex == 2))
			{
                TmpLen.w= MAX_PRO_BUF;//服务器发送帧最大尺寸    long-unsigned
                //pBuf[0] = TmpLen.b[1];//应用层先传高字节
                //pBuf[1] = TmpLen.b[0];
                memcpy( pBuf, TmpLen.b, 2);
                pBuf += 2;
			}

			//最大发送apdu尺寸
			if( (ClassIndex==0) || (ClassIndex==3) )
			{
                TmpLen.w= MAX_PRO_BUF;//服务器发送帧最大尺寸    long-unsigned
                //pBuf[0] = TmpLen.b[1];//应用层先传高字节
                //pBuf[1] = TmpLen.b[0];
                memcpy( pBuf, TmpLen.b, 2);
                pBuf += 2;
			}

			//最大可处理apdu尺寸
			if( (ClassIndex==0) || (ClassIndex==4) )
			{
                TmpLen.w = MAX_APDU_SIZE;//服务器最大可处理apdu最大尺寸    long-unsigned
                //pBuf[0] = TmpLen.b[1];//应用层先传高字节
                //pBuf[1] = TmpLen.b[0];
                memcpy( pBuf, TmpLen.b, 2);
                pBuf += 2;
			}

			//协议一致性块
			if( (ClassIndex==0) || (ClassIndex==5) )
			{
				pBuf += AddProtocolAndFunctionConformance( 1, pBuf );
			}
			
			//功能一致性块
			if( (ClassIndex==0) || (ClassIndex==6) )
			{

			    pBuf += AddProtocolAndFunctionConformance( 2, pBuf );
			}			

	        if( (ClassIndex==0) || (ClassIndex==7) )
			{

			    TmpLen.w = 1800;//超时时间
                pBuf[0] = TmpLen.b[0];
                pBuf[1] = TmpLen.b[1];
                pBuf[2] = 0x00;
			    pBuf[3] = 0x00;
                pBuf += 4;
			}		

			break;

		case 4://当前连接的客户机地址
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}

            pBuf[0] = ClientAddress;

			Len = 1;
			break;

		case 5://连接认证机制
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}

		    pBuf[0] = ConnectMode;

			Len = 1;
    		break;

		default:
			return 0x8000;
	}

	return Len;
}

//--------------------------------------------------
//功能描述:     设备管理（class19）属性数据添加Tag 应用连接
//         
//参数:      
//         	*pOAD[in]：		OAD     
//          *InBuf[in]：	需要添加tag的数据         
//          OutBufLen[in]：	申请的缓冲长度         
//          *OutBuf[out]：	输出数据
//         
//返回值:   添加tag后的数据长度
//         
//备注内容: 处理除属性2外的属性  
//--------------------------------------------------
static WORD AddTagMeterClass20( BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE ClassAttribute,ClassIndex;
	WORD Len,Result;
	const BYTE *Tag;
	
	ClassAttribute = (pOAD[2]&0x1f);//bit0…bit4编码表示对象属性编号；
	ClassIndex = pOAD[3];
	Len = 0x8000;
	
	switch( ClassAttribute )
	{
	    case 2:
	        memcpy( OutBuf, InBuf, sizeof(Array_OI));
	        return sizeof( Array_OI );
	        break;
		case 3://应用语境信息
			if( ClassIndex > T_APPInfo[1] )
			{
				return 0x8000;
			}
			
			Tag = T_APPInfo;
			Tag += GetTagOffsetAddr( ClassIndex, Tag );
			Len = GetTypeLen( eTagData, Tag );
			if( Len == 0x8000 )
			{
                return 0x8000;
			}
			
			if( OutBufLen < Len )
			{
				return 0;
			}
			Result = GetRequestAddTag( Tag, InBuf, OutBuf );
			if( Result == 0x8000 )
			{
				return 0x8000;
			}
			break;

		case 4://当前连接的客户机地址
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			
			Tag = T_Unsigned;
			Len = GetTypeLen( eTagData, Tag );
		    if( Len == 0x8000 )
			{
                return 0x8000;
			}
			
			if( OutBufLen < Len )
			{
				return 0;
			}
			Result = GetRequestAddTag( Tag, InBuf, OutBuf );
			if( Result == 0x8000 )
			{
				return 0x8000;
			}	

        break;
        case 5://连接认证机制
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			
			Tag = T_Enum;
			Len = GetTypeLen( eTagData, Tag );
		    if( Len == 0x8000 )
			{
                return 0x8000;
			}
			
			if( OutBufLen < Len )
			{
				return 0;
			}
			Result = GetRequestAddTag( Tag, InBuf, OutBuf );
			if( Result == 0x8000 )
			{
				return 0x8000;
			}	

        break;

		default:
			return 0x8000;
	}

	return Len;
}

//--------------------------------------------------
//功能描述:     获取电气设备类（class19）数据长度            应用连接
//         
//参数:      
//         	DataType[in]：		DataType[in]：eData/eTagData     
//          *pOAD[in]：			OAD          
//         
//返回值:   添加tag后的数据长度
//         
//备注内容:  
//--------------------------------------------------
static WORD GetMeterClass20Len( BYTE DataType, BYTE *pOAD )
{
	BYTE ClassAttribute,ClassIndex;
	WORD Len;
	const BYTE *Tag;
	
	ClassAttribute = (pOAD[2]&0x1f);//bit0…bit4编码表示对象属性编号；
	ClassIndex = pOAD[3];

	switch( ClassAttribute )
	{
	    case 2:

            return sizeof( Array_OI );
            break;
		case 3://应用语境信息
			if( ClassIndex > T_APPInfo[1] )
			{
				return 0x8000;
			}
			
			Tag = T_APPInfo;
			Tag += GetTagOffsetAddr( ClassIndex, Tag );
			Len = GetTypeLen( DataType, Tag );
			break;

		case 4://当前连接的客户机地址
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			Tag = T_Unsigned;
			Len = GetTypeLen( DataType, Tag );
			break;

		case 5://连接认证机制
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			
			Tag = T_Enum;
			Len = GetTypeLen( DataType, Tag );
			break;
  
		default:
			return 0x8000;
	}
	return Len;
}

//--------------------------------------------------
//功能描述:  地理位置信息（class8）数据添加Tag
//         
//参数:      
//         	*pOAD[in]：		OAD    
//          *InBuf[in]：	需要添加tag的数据         
//          OutBufLen[in]：	申请的缓冲长度         
//          *OutBuf[out]：	输出数据
//         
//返回值:   添加tag后的数据长度
//         
//备注内容: 经度、维度、高度  
//--------------------------------------------------
static WORD AddTagPositionInfo( BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE ClassIndex,Len;
	WORD Result;
	
	Len = 0;
	
	ClassIndex = pOAD[3];
	if( ClassIndex > 3 )
	{
		return 0x8000;
	}
	
	if(ClassIndex == 0)
	{
		if( OutBufLen < 27 )//如果缓冲大小不够，则不进行操作
		{
			return 0;
		}
		Len = 27;
		*(OutBuf++) = Structure_698;
		*(OutBuf++) = 3;
		Result = GetRequestAddTag( T_PositionInfo1, InBuf, OutBuf );
		if( Result == 0x8000 )
		{
			return 0x8000;
		}
		OutBuf +=10;
		Result = GetRequestAddTag( T_PositionInfo1, InBuf+4, OutBuf );
		if( Result == 0x8000 )
		{
			return 0x8000;
		}
		OutBuf +=10;
		Result = GetRequestAddTag( T_UNDoubleLong, InBuf+8, OutBuf );
		if( Result == 0x8000 )
		{
			return 0x8000;
		}
	}
	else if( (ClassIndex==1) || (ClassIndex==2) )
	{
		if( OutBufLen < 10 )//如果缓冲大小不够，则不进行操作
		{
			return 0;
		}
		Len = 10;
		Result = GetRequestAddTag( T_PositionInfo1, InBuf, OutBuf );
		if( Result == 0x8000 )
		{
			return 0x8000;
		}
	}
	else
	{
		if( OutBufLen < 5 )//如果缓冲大小不够，则不进行操作
		{
			return 0;
		}
		
		Len = 5;
		
		Result = GetRequestAddTag( T_UNDoubleLong, InBuf+8, OutBuf );
		if( Result == 0x8000 )
		{
			return 0x8000;
		}

	}

	return Len;
}

//--------------------------------------------------
//功能描述:  时段表（class8）数据添加Tag
//         
//参数:      
//         	*pOAD[in]：		OAD    
//          *InBuf[in]：	需要添加tag的数据         
//          OutBufLen[in]：	申请的缓冲长度         
//          *OutBuf[out]：	输出数据
//         
//返回值:   添加tag后的数据长度
//         
//备注内容: 时段表（按照8个日时段表，14个时段计算长度）  
//--------------------------------------------------
static WORD AddTagTimeTable( BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE ClassIndex,i;
	WORD Result,Len;

	Len = 0x8000;

	ClassIndex = pOAD[3];
	if( ClassIndex > MAX_TIME_SEG_TABLE )
	{
		return 0x8000;
	}

	if( ClassIndex == 0 )
	{
		if( OutBufLen <  (2+8*FPara1->TimeZoneSegPara.TimeSegNum)*FPara1->TimeZoneSegPara.TimeSegTableNum+2)//如果缓冲大小不够，则不进行操作
		{
			return 0;
		}
		Len = ((2+8*FPara1->TimeZoneSegPara.TimeSegNum)*FPara1->TimeZoneSegPara.TimeSegTableNum+2);
		*(OutBuf++) = Array_698;
		*(OutBuf++) = FPara1->TimeZoneSegPara.TimeSegTableNum;
		
		TagPara.Array_Struct_Num = FPara1->TimeZoneSegPara.TimeSegNum;//加TAG按照实际个数进行添加
		
		for( i=0; i < FPara1->TimeZoneSegPara.TimeSegTableNum; i++ )
		{
			Result = GetRequestAddTag( T_Timezone, InBuf, OutBuf );
			if( Result == 0x8000 )
			{
				return 0x8000;
			}

			InBuf += (MAX_TIME_SEG*3);//输入buf按照最大长度进行跳转 请注意-姜文浩
			OutBuf += Result;
		}
	}
	else
	{
		if( OutBufLen < (2+8*FPara1->TimeZoneSegPara.TimeSegNum))//如果缓冲大小不够，则不进行操作
		{
			return 0;
		}
		//Len = (2+8*FPara1->TimeZoneSegPara.TimeSegNum);
		
		TagPara.Array_Struct_Num = FPara1->TimeZoneSegPara.TimeSegNum;

		Result = GetRequestAddTag( T_Timezone, InBuf, OutBuf );
		if( Result == 0x8000 )
		{
			return 0x8000;
		}

		Len = Result;
	}

	return Len;
}

#if( PREPAY_MODE == PREPAY_LOCAL )
//--------------------------------------------------
//功能描述: 	阶梯表（class8）数据添加Tag
//         
//参数:      
//         	*pOAD[in]：		OAD    
//          *InBuf[in]：	需要添加tag的数据         
//          OutBufLen[in]：	申请的缓冲长度         
//          *OutBuf[out]：	输出数据
//         
//返回值:   添加tag后的数据长度
//         
//备注内容: 阶梯表  
//--------------------------------------------------
static WORD AddTagLadder( BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE ClassIndex,i;
	WORD Result,Len;

	Len = 0x8000;

	ClassIndex = pOAD[3];
	if( ClassIndex > 3 )
	{
		return 0x8000;
	}

	if( ClassIndex == 0 )
	{
	    if( OutBufLen < (2+6+(2*MAX_LADDER+1)*5+MAX_YEAR_BILL*8) )
	    {
            return 0x8000;
	    }

	    Len = (2+6+(2*MAX_LADDER+1)*5+MAX_YEAR_BILL*8);
        *(OutBuf++) = Structure_698;
        *(OutBuf++) = 3; 
	}

    if( (ClassIndex == 0) || (ClassIndex == 1) )//阶梯值
    {
        if( ClassIndex == 1 )
        {
            if( OutBufLen < (2+MAX_LADDER*5) )
            {
                return 0x8000;
            }

            Len = (2+MAX_LADDER*5);
        }

        TagPara.Array_Struct_Num = MAX_LADDER;

		Result = GetRequestAddTag( T_Rate, InBuf, OutBuf );
		if( Result == 0x8000 )
		{
			return 0x8000;
		}
		
        InBuf += GetTypeLen( eData, T_Rate );
		OutBuf += Result;
    }
    
    if( (ClassIndex == 0) || (ClassIndex == 2 ) )//阶梯电价
    {
        if( ClassIndex == 2 )
        {
            if( OutBufLen < (2+(MAX_LADDER+1)*5) )
            {
                return 0x8000;
            }

            Len = (2+(MAX_LADDER+1)*5);
        }

        TagPara.Array_Struct_Num = (MAX_LADDER+1);

		Result = GetRequestAddTag( T_Rate, InBuf, OutBuf );
		if( Result == 0x8000 )
		{
			return 0x8000;
		}
		
        InBuf += GetTypeLen( eData, T_Rate );
		OutBuf += Result;
    }
    
    if( (ClassIndex == 0) || (ClassIndex == 3) )//阶梯结算日
    {
        if( ClassIndex == 3 )
        {
            if( OutBufLen < (2+(MAX_YEAR_BILL)*8) )
            {
                return 0x8000;
            }

            Len = (2+(MAX_YEAR_BILL)*8);
        }

        *(OutBuf++) = Array_698;
        *(OutBuf++) = MAX_YEAR_BILL;
        
        TagPara.Array_Struct_Num = MAX_YEAR_BILL;

        for( i=0; i<MAX_YEAR_BILL; i++ )
        {
    		Result = GetRequestAddTag( T_YearBill, InBuf, OutBuf );
    		if( Result == 0x8000 )
    		{
    			return 0x8000;
    		}

            InBuf += GetTypeLen( eData, T_YearBill );
    		OutBuf += Result;
        }
		
    }

    return Len;
}
#endif
//--------------------------------------------------
//功能描述:  获取电表基本信息的枚举号（TMeterTypeEnum）
//         
//参数:      OI[in]：OI
//         
//返回值:    电表基本信息的枚举号
//         
//备注内容:  无
//--------------------------------------------------
static BYTE GetProMeterType( WORD OI )
{
	switch( OI )
	{
		case 0x4001://通讯地址
			return eMeterCommAddr;
			break;
		case 0x4002://表号
			return eMeterMeterNo;
			break;
		case 0x4003://客户编号
			return eMeterCustomCode;
			break;
		case 0x4004://地理位置信息
			return eMeterMeterPosition;
			break;
		case 0x4103://资产管理编码
			return eMeterPropertyCode;
			break;
		case 0x4104://额定电压
			return eMeterRateVoltage;
			break;
		case 0x4105://额定电流/基本电流
			return eMeterRateCurrent;
			break;
		case 0x4106://最大电流
			return eMeterMaxCurrent;
			break;
		case 0x4107://有功准确等级
			return eMeterPPrecision;
			break;
		case 0x4108://无功准确等级
			return eMeterQPrecision;
			break;
		case 0x4109://电能表有功常数
			return eMeterActiveConstant;
			break;
		case 0x410A://电能表无功常数
			return eMeterReactiveConstant;
			break;
		case 0x410B://电能表型号
			return eMeterMeterModel;
			break;
		case 0x4111://软件备案号
			return eMeterSoftRecord;
			break;
		case 0x4300://软件备案号
			return eMeterProduceDate;
			break;
		default:
			return 0x80;
	}
}

//--------------------------------------------------
//功能描述: 获取lcd参数
//         
//参数:      
//         	ClassIndex[in]：元素索引
//          Sch[in] ：		ParaMeterObj中的索引        
//          *pBuf[out]：		输出缓冲
//         
//返回值:   数据长度
//         
//备注内容: OI=4007 LCD参数 不含tag
//--------------------------------------------------
static WORD GetProLcdPara( BYTE ClassIndex, BYTE Sch, BYTE *pBuf )
{
	WORD Len,Len2;
	
	Len2= GetTypeLen(eData, ParaMeterObj[Sch].Type2);
    if( Len2 == 0x8000 )
	{
        return 0x8000;
	}
	
	Len = 0;
	memcpy( pBuf, (BYTE*)&(FPara1->LCDPara.PowerOnDispTimer), Len2 );
	
    Len = Len2;
    
	if( ClassIndex != 0 )
	{
		switch( ClassIndex )
		{
			case 0x01://上电全显时间  unsigned，
				memcpy( pBuf,(BYTE*)&(FPara1->LCDPara.PowerOnDispTimer),sizeof(BYTE) );
				Len = sizeof(BYTE);
				break;		
			case 0x02://背光点亮时间  long-unsigned(按键时背光点亮时间)，
				memcpy( pBuf,(BYTE*)&(FPara1->LCDPara.BackLightPressKeyTimer),sizeof(WORD) );
				Len = sizeof(WORD);
				break;
			case 0x03://显示查看背光点亮时间  long-unsigned，
				memcpy( pBuf,(BYTE*)&(FPara1->LCDPara.BackLightViewTimer),sizeof(WORD) );
				Len = sizeof(WORD);
				break;
			case 0x04://无电按键屏幕驻留最大时间  long-unsigned， HEX 单位为秒
				memcpy( pBuf,(BYTE*)&(FPara1->LCDPara.LcdSwitchTime),sizeof(WORD) );
				Len = sizeof(WORD);
				break;
			case 0x05://电能小数点
				memcpy( pBuf,(BYTE*)&(FPara1->LCDPara.EnergyFloat),sizeof(BYTE) );
				Len = sizeof(BYTE);
				break;
			case 0x06://功率（需量）小数点
				memcpy( pBuf,(BYTE*)&(FPara1->LCDPara.DemandFloat),sizeof(BYTE) );
				Len = sizeof(BYTE);
				break;
			case 0x07://显示12字样意义  0:用于显示当前套、备用套时段 1：用于显示当前套、备用套费率电价
				memcpy( pBuf,(BYTE*)&(FPara1->LCDPara.Meaning12),sizeof(BYTE) );
				Len = sizeof(BYTE);
				break;
			default:
			    Len = 0x8000;
				break;
		}
	}

	return Len;
}
WORD GetApplianceFreezData(BYTE Index, BYTE Count, BYTE *Buffer )
{	
	BYTE Num;//读取的个数
	BYTE i;//取数据起始点
	
	if(Index == 0)
	{
		Num = Count;
		i = 0;
	}
	else
	{
		Num = 1;
		i = Index - 1;
	}
	
	memcpy(Buffer, &MessageData.ucData[i], Num * sizeof(T_ApplFrzData));
	
	return Num * sizeof(T_ApplFrzData);
}
WORD GetFrzorCurLen(BYTE flag )
{	
	BYTE i,res=0;
	T_ApplCurData tempBuf = {0xFFFF,0xFF,0xFFFFFFFF};
	T_ApplFrzData tempBuf1 = {0xFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF,0xFF,0xFF};
	
	for(i=0;i<NILM_EQUIPMENT_MAX_NO;i++)
	{
		if(flag == 2)
		{
			res = NILM_EQUIPMENT_TYPE_NO;
			return res;
		}
		// else if(flag == 3)
		// {
		// 	if(memcmp(&tempBuf.ApplClassID, &MessageData[i].ApplClassID, sizeof(T_ApplCurData)) == 0)
		// 	{
		// 		continue;
		// 	}
		// }
		else if (flag == 4)
		{
			if(memcmp(&tempBuf1.ApplClassID, &MessageData.ucData[i].ApplClassID, sizeof(T_ApplFrzData)) == 0)
			{
				continue;
			}
		}
		res++;
	}
	
	return res;
}
//--------------------------------------------------
//功能描述:  获取参变量（class8）数据
//         
//参数:     
//          *pOAD[in]：	OAD         
//          Sch[in]：   ParaMeterObj 中的数据索引      
//          *pBuf[out]：输出数据
//         
//返回值:   数据长度
//         
//备注内容: 不含tag
//--------------------------------------------------
static WORD GetProMeterParaData( BYTE *pOAD, BYTE Sch, BYTE *pBuf )
{
	TTwoByteUnion OI;
	BYTE ClassAttribute,ClassIndex,Num,i,MaxIndex;
	WORD Type,Result,Len;
	BYTE *pBufBak = pBuf;
	const BYTE *ObjType;
	TTimeBroadCastPara tmpTimeBroadCastPara;
	#if( PREPAY_MODE == PREPAY_LOCAL)
	TRatePrice				RatePrice;		//当前费率电价表
    TLadderPrice			LadderPrice;	//当前阶梯电价表
    #endif
    
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);//bit0…bit4编码表示对象属性编号；
	ClassIndex = pOAD[3];
	ObjType = ParaMeterObj[Sch].Type2;
	Len = 0;
	
	if( OI.w == 0x4300 )//电气设备接口类 单独处理
	{
		return GetMeterClass19Data( pOAD, pBuf );
	}
	else if( OI.w == 0x4400 )//应用连接接口类  应用连接
	{
        return GetMeterClass20Data( pOAD, pBuf );
	}
	
	switch( ClassAttribute )
	{
		case 2:
			switch( OI.w )
			{
				case 0x4000://日期时间
					if( ClassIndex != 0)
					{
						return 0x8000;
					}
					Len = GetTypeLen( eData, ObjType );
        			if( Len == 0x8000 )
        			{
                        return 0x8000;
        			}
        			
					Result = api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss, pBuf );
					if(Result == FALSE)
					{
						return 0x8000;
					}

					pBuf += Len;					
					break;

				case 0x4001://通讯地址
				case 0x4002://表号
				case 0x4003://客户编号
				case 0x4103://资产管理编码
				case 0x4104://额定电压
				case 0x4105://额定电流/基本电流
				case 0x4106://最大电流
				case 0x4107://有功准确等级
				case 0x4108://无功准确等级
				case 0x4109://电能表有功常数
				case 0x410A://电能表无功常数
				case 0x410B://电能表型号
				case 0x4111://软件备案号
					if( ClassIndex != 0 )
					{
						return 0x8000;
					}
					
					Type = GetProMeterType( OI.w );
					if( Type == 0x80 )
					{
						return 0x8000;
					}
					
					Len = GetTypeLen( eData, ObjType );
        			if( Len == 0x8000 )
        			{
                        return 0x8000;
        			}
        			
					Result = api_ProcMeterTypePara( READ, Type, pBuf );
					if(Result == FALSE)
					{
						return 0x8000;
					}
					pBuf += Len;
					break;
				case 0x4004://地理位置信息
					if( ClassIndex > 3 )
					{
						return 0x8000;
					}
					
					Type = GetProMeterType( OI.w );
					if( Type == 0x80 )
					{
						return 0x8000;
					}
					
					Len = (ClassIndex == 0) ? 12 : 4;
					Result = api_ProcMeterTypePara( READ, Type, pBuf );
					if(Result == FALSE)
					{
						Len = 0x8000;
						break;
					}
					if( ClassIndex !=0 )
					{
						memcpy(pBuf,pBuf+(ClassIndex - 1)*4 ,4);
					}
					pBuf += Len;
					break;
				case 0x4007://LCD参数
					if( ClassIndex > ObjType[1] )
					{
						return 0x8000;
					}
					Len = GetProLcdPara(ClassIndex,  Sch, pBuf );
					if( Len == 0x8000 )
					{
						return 0x8000;
					}
					pBuf += Len;
					break;

				case 0x4008://备用套时区切换时间
				case 0x4009://备用套日时段表切换时间
				#if(PREPAY_MODE == PREPAY_LOCAL)
				case 0x400a://备用套分时费率切换时间
				case 0x400b://备用套阶梯电价切换时间
				#endif
					if( ClassIndex != 0 )
					{
						return 0x8000;
					}
					Len = GetTypeLen( eData, ObjType);
        			if( Len == 0x8000 )
        			{
                        return 0x8000;
        			}
        			
					//Result = GetProSwitchTime(OI.w, pBuf);
					if( api_ReadAndWriteSwitchTime( READ, OI.w - 0x4008, pBuf ) == FALSE )
					//if(Result == FALSE)
					{
						return 0x8000;
					}
					
                    pBuf[6] = 0XFF;

					pBuf += Len;
					break;

				case 0x400C://时区时段数
					if( ClassIndex > ObjType[1] )
					{
						return 0x8000;
					}
					ObjType += GetTagOffsetAddr( ClassIndex, ObjType);
					Len = GetTypeLen( eData, ObjType );
					if( Len == 0x8000 )
        			{
                        return 0x8000;
        			}
        			
					memcpy( pBuf, ((BYTE*)&FPara1->TimeZoneSegPara.TimeZoneNum), sizeof(TTimeZoneSegPara));

					if( ClassIndex != 0 )
					{
						memcpy( pBuf, pBuf+(ClassIndex-1), Len);
					}

					pBuf += Len;
					break;

				case 0x400F://密钥总条数
				case 0x401c://CT
				case 0x401d://PT
					if( ClassIndex != 0 )
					{
						return 0x8000;
					}
					
					if( OI.w == 0x400F )
					{
						Type = eKeyNum;
					}
					else if( OI.w == 0x401c )
					{
						Type = eCTCoe;
					}
					else
					{
						Type = ePTCoe;
					}
					
					Len = GetTypeLen( eData, ObjType );
					if( Len == 0x8000 )
        			{
                        return 0x8000;
        			}
        			
					api_ReadPrePayPara((ePrePayParaType)Type,pBuf);
					pBuf += Len;
					break;

				case 0x4010://计量元件数
					if( ClassIndex != 0 )
					{
						return 0x8000;
					}
					
					#if(MAX_PHASE == 1)
					*(pBuf++) = 1;
					#else
					if(MeterTypesConst == METER_3P3W)
					{
						*(pBuf++) = 2;
					}
					else
					{
						*(pBuf++) = 3;
					}
					#endif
					break;
				case 0x4011://公共假日表
				case 0x4014://当前套时区表
				case 0x4015://备用套时区表					
					if( OI.w == 0x4011 )
					{
						if( ClassIndex > MAX_HOLIDAY )
						{
							return 0x8000;
						}
						Type = 0x02;
						TagPara.Array_Struct_Num = FPara1->TimeZoneSegPara.HolidayNum;
					}
					else if( OI.w == 0x4014 )
					{
						if( ClassIndex > MAX_TIME_AREA )
						{
							return 0x8000;
						}
						Type = 0x01;
						TagPara.Array_Struct_Num = MAX_TIME_AREA;//FPara1->TimeZoneSegPara.TimeZoneNum;
					}
					else
					{
						if( ClassIndex > MAX_TIME_AREA )
						{
							return 0x8000;
						}
						Type = 0x81;
						TagPara.Array_Struct_Num = MAX_TIME_AREA;//FPara1->TimeZoneSegPara.TimeZoneNum;
					}					
					
					ObjType += GetTagOffsetAddr( ClassIndex, ObjType);
					Len = GetTypeLen( eData, ObjType );
        			if( Len == 0x8000 )
        			{
                        return 0x8000;
        			}
        			
					if( ClassIndex == 0 )
					{
						ClassIndex = 1;
					}
					
					Result = api_ReadTimeTable(Type, Len*(ClassIndex-1), Len , pBuf);
					pBuf += Len;
					break;
				
				case 0x4016://当前套日时段表
				case 0x4017://备用套时段表
					if( ClassIndex > MAX_TIME_SEG_TABLE )
					{
						return 0x8000;
					}
					Type = ( OI.w == 0x4016) ? 0x03 : 0x83;
					i = (( ClassIndex == 0) ? 0 : (ClassIndex-1));
					//Num = (( ClassIndex == 0) ? FPara1->TimeZoneSegPara.TimeSegTableNum : 1);
					Num = (( ClassIndex == 0) ? MAX_TIME_SEG_TABLE : 1);
					Num += i;
					
					if( ClassIndex == 0 )
					{
						ClassIndex = 1;
					}
					
					for( ; i < Num; i++ )
					{
						//Result = api_ReadTimeTable(Type+i, 0, 3*FPara1->TimeZoneSegPara.TimeSegNum, pBuf);
						//pBuf += (3*FPara1->TimeZoneSegPara.TimeSegNum);
						Result = api_ReadTimeTable(Type+i, 0, 3*MAX_TIME_SEG, pBuf);
						pBuf += (3*MAX_TIME_SEG);
					}

					break;
				case 0x4012://周休日特征字
				case 0x4013://周休日采用的日时段表号
					if( ClassIndex != 0 )
					{
						return 0x8000;
					}

					if( OI.w == 0x4012 )
					{
						*(pBuf++) = FPara1->TimeZoneSegPara.WeekStatus;
					}
					else
					{
						*(pBuf++) = FPara1->TimeZoneSegPara.WeekSeg;
					}
	
					break;
			    #if( PREPAY_MODE == PREPAY_LOCAL)
                case 0x400d://阶梯数
                    if( ClassIndex != 0 )
                    {
                        return 0x8000;
                    }

                    api_ReadPrePayPara( eLadderNum, pBuf );
                    pBuf++;
                    
                    break;

                case 0x4018://当前套费率电价
                case 0x4019://备用套费率电价
                    if( ClassIndex > FPara1->TimeZoneSegPara.Ratio )
                    {
                        return 0x8000;
                    }
					if( OI.w == 0x4018 )
					{
						api_ReadPrePayPara(eCurRateTable, (BYTE*)RatePrice.Price);
					}
					else
					{
						api_ReadPrePayPara(eBackupRateTable, (BYTE*)RatePrice.Price);
					}

                    TagPara.Array_Struct_Num = (FPara1->TimeZoneSegPara.Ratio);
                    
                    if( ClassIndex == 0 )
                    {
                        memcpy( pBuf, (BYTE*)RatePrice.Price, 4*(FPara1->TimeZoneSegPara.Ratio) );
                        pBuf += (4*(FPara1->TimeZoneSegPara.Ratio));
                    }
                    else
                    {
                        memcpy( pBuf, (BYTE*)&RatePrice.Price[ClassIndex-1], 4 );
                        pBuf += 4;
                    }
                    
                    break;

                case 0x401A://当前套阶梯电价
                case 0x401B://备用套阶梯电价
                    if( ClassIndex > 3 )
                    {
                        return 0x8000;
                    }
					if( OI.w == 0x401A )
					{
						api_ReadPrePayPara(eCurLadderTable, (BYTE*)LadderPrice.Ladder_Dl );
					}
					else
					{
						api_ReadPrePayPara(eBackupLadderTable, (BYTE*)LadderPrice.Ladder_Dl );
					}

                    if( ClassIndex == 0 )
                    {
                        memcpy( pBuf, (BYTE*)LadderPrice.Ladder_Dl, (sizeof(TLadderPrice)-sizeof(DWORD)) );    
                        pBuf += (sizeof(TLadderPrice)-sizeof(DWORD));
                    }
                    else
                    {
                        if( ClassIndex == 1 )//阶梯值
                        {
                            memcpy( pBuf, (BYTE*)LadderPrice.Ladder_Dl, sizeof(LadderPrice.Ladder_Dl) );
                            pBuf += sizeof(LadderPrice.Ladder_Dl);
                        }
                        else if( ClassIndex == 2 )//阶梯电价
                        {
                            memcpy( pBuf, (BYTE*)LadderPrice.Price, sizeof(LadderPrice.Price) );
                            pBuf += sizeof(LadderPrice.Price);
                        }
                        else
                        {
                            memcpy( pBuf, (BYTE*)LadderPrice.YearBill, sizeof(LadderPrice.YearBill) );
                            pBuf += sizeof(LadderPrice.YearBill);
                        }
                    }
                    break;
                case 0x401e://报警金额限值
                case 0x401f://其他金额限值
                    if( OI.w == 0x401e )
                    {
                        if( ClassIndex > 2 )
                        {
                            return 0x8000;
                        }

                        Num = ((ClassIndex == 0) ? 2 : 1 );
                        Type = ((ClassIndex == 0) ? eAlarm_Limit1 : (eAlarm_Limit1+(ClassIndex-1)) );
                    }
                    else
                    {
                        if( ClassIndex > 3 )
                        {
                            return 0x8000;
                        }
                        
                        Num = ((ClassIndex == 0) ? 3 : 1 );
                        Type = ((ClassIndex == 0) ? eTickLimit : (eTickLimit+(ClassIndex-1)) );
                    }

                    for( i=0; i<Num; i++ )
                    {
                        api_ReadPrePayPara( (ePrePayParaType)(Type+i), pBuf );
                        pBuf += 4;
                    }
                    
                    break;
        		case 0x4022://T_BitString16	},//插卡状态字
        			if( ClassIndex != 0 )//如果缓冲大小不够，则不进行操作
					{
						return 0x8000;
					}
                    Len = GetTypeLen( eData, ObjType );
					if( Len == 0x8000 )
        			{
                        return 0x8000;
        			}
					api_GetCardInsertStatus(pBuf);
					pBuf += Len;
                    
                    break;	

                #endif
				case 0x4030://电压合格率参数
					if( ClassIndex > ObjType[1] )
					{
						return 0x8000;
					}
					
					ObjType += GetTagOffsetAddr( ClassIndex, ObjType);
					Len = GetTypeLen( eData, ObjType );
					if( Len == 0x8000 )
        			{
                        return 0x8000;
        			}
        			
					Result = api_ReadEventPara( OI.w, ClassIndex, (BYTE*)&Len, pBuf );
					if(Result == FALSE)
					{
						return 0x8000;
					}

					pBuf += Len;
					break;

				#if( MAX_PHASE == 3 )
				case 0x4100://最大需量周期
				case 0x4101://滑差时间
					if( ClassIndex != 0 )//如果缓冲大小不够，则不进行操作
					{
						return 0x8000;
					}

					if( OI.w == 0x4100 )
					{
						*(pBuf++) = FPara2->EnereyDemandMode.DemandPeriod;
					}
					else if( OI.w == 0x4101 )
					{
						*(pBuf++) = FPara2->EnereyDemandMode.DemandSlip;
					}
					break;
				#endif
				case 0x4102://校表脉冲宽度
					if( ClassIndex != 0 )//如果缓冲大小不够，则不进行操作
					{
						return 0x8000;
					}
					*(pBuf++) = 80;
					break;

				case 0x4112://有功组合方式特征字
					if( ClassIndex != 0 )//如果缓冲大小不够，则不进行操作
					{
						return 0x8000;
					}
					*(pBuf++) = FPara2->EnereyDemandMode.byActiveCalMode;
					break;
				
				#if(SEL_RACTIVE_ENERGY == YES)
				case 0x4113://无功组合方式特征字1
				case 0x4114://无功组合方式特征字2
					if( ClassIndex != 0 )//如果缓冲大小不够，则不进行操作
					{
						return 0x8000;
					}

					if( OI.w == 0x4113 )
					{
						*(pBuf++) = FPara2->EnereyDemandMode.PReactiveMode;
					}
					else
					{
						*(pBuf++) = FPara2->EnereyDemandMode.NReactiveMode;
					}
					break;
				#endif

				case 0x4116://结算日
					if( ClassIndex > MAX_MON_CLOSING_NUM )
					{
						return 0x8000;
					}
					
					TagPara.Array_Struct_Num = MAX_MON_CLOSING_NUM;
					ObjType += GetTagOffsetAddr( ClassIndex, ObjType);
					Len = GetTypeLen( eData, ObjType );
					if( Len == 0x8000 )
        			{
                        return 0x8000;
        			}
        			
					Result =ReadSettlementDate(pBuf);
					if(Result == FALSE)
					{
						return 0x8000;
					}

					if( ClassIndex != 0 )
					{
						memcpy(pBuf,pBuf+(ClassIndex-1)*2,2);
						pBuf += 2;
					}
					else
					{
						pBuf+=Len;// 0的时候返回全部长度
					}
					break;

				#if( MAX_PHASE == 3 )
				case 0x4117://期间需量冻结周期
					if( ClassIndex != 0 )
					{
						return 0x8000;
					}
					Len = GetTypeLen( eData, ObjType );
					if( Len == 0x8000 )
        			{
                        return 0x8000;
        			}
        			
					memcpy(pBuf,(BYTE*)&FPara2->EnereyDemandMode.IntervalDemandFreezePeriod,3);
					pBuf += Len;
					break;
				#endif
				default:
					return 0x8000;
			}
			break;
		case 3:
		case 4:
			if( OI.w == 0x4106 )//计量规格
			{
				
				if( ClassIndex != 0 )
				{
					return 0x8000;
				}
				if(ClassAttribute == 3)
				{
					Type = eMeterMinCurrent;
				}
				else
				{
					Type = eMeterTurningCurrent;
				}
				Len = GetTypeLen( eData, ObjType );
    			if( Len == 0x8000 )
    			{
                    return 0x8000;
    			}
    			
				Result = api_ProcMeterTypePara( READ, Type, pBuf );
				if(Result == FALSE)
				{
					return 0x8000;
				}
				pBuf += Len;
			}
			else if( OI.w == 0x4800 )///属性4/电器设备冻结数据单元
			{
				MaxIndex = GetFrzorCurLen(4);	//一次识别电器的设备个数
				if(ClassIndex != 0)				//index非零情况
				{
					return 0x8000;
				}
				ObjType = (BYTE *)&A_4800_4_ApplFrzData_NotElecHot;
				Len = GetTypeLen( eData, ObjType );
				if( Len == 0x8000 )
				{
					return 0x8000;
				}
				
				Result = TRUE;
				Len = GetApplianceFreezData(ClassIndex, MaxIndex, pBuf);
				pBuf += Len;
			}
			else
			{
				return 0x8000;
			}
			break;
		case 5:
			if( OI.w == 0x4000 )//广播校时参数
			{
				if( ClassIndex > T_TimeBroadCastPara[1] )
				{
					return 0x8000;
				}
				
				api_VReadSafeMem( GET_SAFE_SPACE_ADDR( ProSafeRom.TimeBroadCastPara), sizeof(TTimeBroadCastPara), (BYTE*)&tmpTimeBroadCastPara ); 
				if( ClassIndex == 0 )
				{
					memcpy(pBuf,(BYTE*)&tmpTimeBroadCastPara,4);
					pBuf += 4;			
				}
				else if( ClassIndex == 1 )//设置广播校时最小响应限制
				{
					memcpy(pBuf,(BYTE*)&tmpTimeBroadCastPara.TimeMinLimit,2);
					pBuf += 2;
				}
				else//读取广播校时最大响应限制
				{
					memcpy(pBuf,(BYTE*)&tmpTimeBroadCastPara.TimeMaxLimit,2);
					pBuf += 2;
				}
			}
			else
			{
				return 0x8000;
			}
			break;

		default:
			return 0x8000;
	}

	return  (WORD)(pBuf-pBufBak);
}

//--------------------------------------------------
//功能描述:  对参变量（class8）数据添加Tag
//         
//参数:      
//         	*pOAD[in]：		OAD    
//          *InBuf[in]：	需要添加tag的数据         
//          OutBufLen[in]：	申请的缓冲长度         
//          *OutBuf[out]：	输出数据
//         
//返回值:   添加tag后的数据长度
//         
//备注内容: 参变量（class8）  OI=4XXX
//--------------------------------------------------
static WORD AddTagMeterPara(BYTE *pOAD, BYTE Sch, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE ClassAttribute,ClassIndex;
	const BYTE *Tag;
	TTwoByteUnion OI;
	WORD Result,Lenth;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);//bit0…bit4编码表示对象属性编号；
	ClassIndex = pOAD[3];
	Lenth = 0;

	if( OI.w == 0x4300 )//电气设备类
	{
		return (AddTagMeterClass19( pOAD, InBuf, OutBufLen, OutBuf));
	}
	else if( OI.w == 0x4400 )//应用连接接口类
	{
        return (AddTagMeterClass20( pOAD, InBuf, OutBufLen, OutBuf));
	}
	
	switch( ClassAttribute )
	{
		case 2://属性2
			if( OI.w == 0x4004 )//地理位置信息
			{
				return (AddTagPositionInfo( pOAD, InBuf, OutBufLen, OutBuf));
			}
			else if( (OI.w == 0x4016) || (OI.w == 0x4017))//时段表
			{
				return (AddTagTimeTable(pOAD,InBuf, OutBufLen,OutBuf));
			}
			#if(PREPAY_MODE == PREPAY_LOCAL)  
			else if( (OI.w == 0x401A) || (OI.w == 0x401B) )//阶梯电价
			{
                return (AddTagLadder( pOAD, InBuf, OutBufLen, OutBuf) );

			}
			#endif
			else if( 	(OI.w == 0x4007)	//LCD参数 
					|| 	(OI.w == 0x400C) 	//时区时段数
					|| 	(OI.w == 0x4011)	//公共假日表
					|| 	(OI.w == 0x4014)	//当前套时区表
					|| 	(OI.w == 0x4015) 	//备用套时区表
					#if(PREPAY_MODE == PREPAY_LOCAL)  
                    || 	(OI.w == 0x4018)    //当前套费率电价
                    || 	(OI.w == 0x4019)    //备用套费率电价
                    || 	(OI.w == 0x401e)    //0x401e:报警金额限值
					|| 	(OI.w == 0x401f)    //0x401f:其他金额限值 
                    #endif//#if(PREPAY_MODE == PREPAY_LOCAL) 
					|| 	(OI.w == 0x4116)	//结算日
					|| 	(OI.w == 0x4030))	//电压合格率
			{

				if( OI.w == 0x4011 )//公共假日表
				{
					TagPara.Array_Struct_Num = FPara1->TimeZoneSegPara.HolidayNum;
					if( ClassIndex > MAX_HOLIDAY )
					{
						return 0x8000;
					}
				}
				else if( (OI.w == 0x4014) || (OI.w == 0x4015) )//时区表
				{
					TagPara.Array_Struct_Num = FPara1->TimeZoneSegPara.TimeZoneNum;
					if( ClassIndex > MAX_TIME_AREA )
					{
						return 0x8000;
					}
				}
				#if(PREPAY_MODE == PREPAY_LOCAL)  
				else if( (OI.w == 0x4018) || (OI.w == 0x4019) )//当前套费率电价、备用套费率电价
				{
                    TagPara.Array_Struct_Num = FPara1->TimeZoneSegPara.Ratio;
					if( ClassIndex > FPara1->TimeZoneSegPara.Ratio )
					{
						return 0x8000;
					}
				}
				#endif
				else if( OI.w == 0x4116 )//结算日
				{
					TagPara.Array_Struct_Num = MAX_MON_CLOSING_NUM;
					if( ClassIndex > MAX_MON_CLOSING_NUM )
					{
						return 0x8000;
					}
				}
				else
				{
                    if( ClassIndex > ParaMeterObj[Sch].Type2[1] )
                    {
                        return 0x8000;
                    }
				}
				
				Tag = ParaMeterObj[Sch].Type2;
				Tag +=GetTagOffsetAddr( ClassIndex, Tag);
				Lenth = GetTypeLen( eTagData, Tag );
                if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
        			
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				Result = GetRequestAddTag( Tag, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}

				return Lenth;
			}
			else//普通数据
			{
				if( ClassIndex != 0 )
				{
					return 0x8000;
				}
				Tag = ParaMeterObj[Sch].Type2;
				Lenth = GetTypeLen( eTagData, Tag) ;
				if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
                
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				
				Result = GetRequestAddTag( Tag, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}
				return Lenth;		
			}
			break;
		case 3:
		case 4:	
			if( OI.w == 0x4106 )//计量规格参数
			{
				if( ClassIndex != 0 )
				{
					return 0x8000;
				}
				Tag =T_MinI;
            	TagPara.Lenth.w = InBuf[0];
            	Lenth = GetTypeLen( eTagData, Tag );
				if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
                
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				
				Result = GetRequestAddTag( Tag, InBuf+1, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}
				return Lenth;
			}
			else if( OI.w == 0x4800 )//Tag属性4
			{
				//TagPara.Array_Struct_Num = MessageData.ucMessageID;
				TagPara.Array_Struct_Num = NumofNILM;
				if( ClassIndex > TagPara.Array_Struct_Num)
				{
					return 0x8000;
				}

				if(ClassIndex != 0)		//index非零情况
				{
					return 0x8000;
				}
				Tag = (BYTE *)&A_4800_4_ApplFrzData;		//临时
				Tag +=GetTagOffsetAddr( ClassIndex, Tag);
				Lenth = GetTypeLen4804( eTagData, Tag, InBuf);//特殊处理
				if( Lenth == 0x8000 )
				{
					return 0x8000;
				}
					
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				Result = GetRequestAddTag4804( Tag, InBuf, OutBuf );//特殊处理!!!!!!
				if( Result == 0x8000 )
				{
					return 0x8000;
				}

				return Lenth;

			}
			else
			{
				return 0x8000;
			}
			break;

		case 5:
			if( OI.w == 0x4000 )//广播校时参数
			{
				if( ClassIndex > T_TimeBroadCastPara[1] )
				{
					return 0x8000;
				}
				
				Tag = T_TimeBroadCastPara;
				Tag +=GetTagOffsetAddr( ClassIndex, Tag);
				Lenth = GetTypeLen( eTagData, Tag );
                if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
        			
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				
				Result = GetRequestAddTag( Tag, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}

				return Lenth;
			}
			else
			{
				return 0x8000;
			}
			break;
		default:
			return 0x8000;
	}	
}

//--------------------------------------------------
//功能描述:  读取参变量（class8）数据 OI=4XXX
//         
//参数:  
//         	DataType[in]：	eData/eTagData/eAddTag
//         	Dot[in]:		小数点(暂不用)        
//         	*pOAD[in]：		OAD         
//         	*InBuf[in]：	需要添加tag的数据         
//         	OutBufLen[in]：	申请的缓冲长度         
//         	*OutBuf[out]：	输出数据
//         
//返回值:  	返回数据长度
//         
//备注内容: 
//--------------------------------------------------
WORD GetRequestMeterPara( BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{	
	BYTE Sch;
	BYTE Buf[MAX_PARA_BUF_LENTH];
	TTwoByteUnion OI;
	WORD Lenth;
	
	lib_ExchangeData(OI.b,pOAD,2);
	
	Sch = SearchParaMeterOAD( OI.w );
	if( Sch == 0x80 )
	{
		return 0x8000;
	}

	if( (DataType==eData) || (DataType==eTagData) )
	{
		Lenth = GetProMeterParaData( pOAD, Sch, Buf );
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
			Lenth = AddTagMeterPara(pOAD, Sch, Buf, OutBufLen, OutBuf);
		}
	}
	else if( DataType == eAddTag )
	{
		if( InBuf == NULL )
		{
			return 0x8000;
		}
		Lenth = AddTagMeterPara(pOAD, Sch, InBuf, OutBufLen, OutBuf);
	}
	else
	{
		return 0x8000;
	}

	return Lenth; 
}

//--------------------------------------------------
//功能描述: 读取参变量（class8）数据长度 OI=4XXX
//         
//参数:     
//         	DataType[in]：	eData/eTagData
//         	*pOAD[in]：		OAD   
//         
//返回值:   返回数据长度 
//         
//备注内容:  无
//--------------------------------------------------
WORD GetRequestMeterParaLen( BYTE DataType, BYTE* pOAD)
{
	BYTE ClassAttribute,Sch,ClassIndex;
	TTwoByteUnion OI,Len;	
	const BYTE *Tag;
	
	Len.w = 0;
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);//bit0…bit4编码表示对象属性编号；
	ClassIndex = pOAD[3];
	Sch = SearchParaMeterOAD( OI.w );
	if( Sch == 0x80 )
	{
		return 0x8000;
	}
	
	if( OI.w == 0x4300 )//电气设备类
	{
		return GetMeterClass19Len(DataType, pOAD);
	}
	else if( OI.w == 0x4400 )//应用连接接口类
	{
        return GetMeterClass20Len(DataType, pOAD);
	}
	
	switch( ClassAttribute )
	{
		case 2://总及费率最大需量数组
			if( OI.w == 0x4004 )//地理位置信息
			{
				if( ClassIndex > 3 )
				{
					return 0x8000;
				}
				
				if( ClassIndex == 0 )
				{
					Len.w = ( DataType == eData) ? 12 : 27 ;
				}
				else if( ClassIndex < 3 )
				{
					Len.w = ( DataType == eData) ? 4 :10 ;
				}
				else
				{
					Len.w = ( DataType == eData) ? 4 : 5;
				}
			}
			else if( (OI.w == 0x4016) || (OI.w == 0x4017))//时段表
			{
				if( ClassIndex > MAX_TIME_SEG_TABLE )//支持先改时段表后改时段数的编程记录
				{
					return 0x8000;
				}

				if( DataType == eData )//源数据按最大长度传，加tag按实际长度加tag
				{
					//Len.w = ( ClassIndex== 0) ? (FPara1->TimeZoneSegPara.TimeSegTableNum*FPara1->TimeZoneSegPara.TimeSegNum*3) : FPara1->TimeZoneSegPara.TimeSegNum*3;
                    Len.w = ( ClassIndex== 0) ? ( MAX_TIME_SEG_TABLE*MAX_TIME_SEG*3) : (MAX_TIME_SEG*3);
				}
				else
				{
					Len.w = ( ClassIndex == 0) ? ((2+8*FPara1->TimeZoneSegPara.TimeSegNum)*FPara1->TimeZoneSegPara.TimeSegTableNum+2) : (2+8*FPara1->TimeZoneSegPara.TimeSegNum);
					//Len.w = ( ClassIndex == 0) ? ((2+8*MAX_TIME_SEG)*MAX_TIME_SEG_TABLE+2) : (2+8*MAX_TIME_SEG);
				}
			}
			#if(PREPAY_MODE == PREPAY_LOCAL)  
			else if( (OI.w == 0x401A) || (OI.w == 0x401B) )//阶梯电价
			{
                if( ClassIndex == 0 )
                {
                    Len.w = (( DataType== eData) ? ((2*MAX_LADDER+1)*4+MAX_YEAR_BILL*3) : (2+6+(2*MAX_LADDER+1)*5+MAX_YEAR_BILL*8));
                }
                else if( ClassIndex == 1 )//阶梯值
                {
                    Len.w = (( DataType== eData) ? (MAX_LADDER*4) : (2+MAX_LADDER*5) );
                }
                else if( ClassIndex == 2 )//阶梯电价
                {
                    Len.w = (( DataType== eData) ? ((MAX_LADDER+1)*4) : (2+(MAX_LADDER+1)*5) );
                }
                else if( ClassIndex == 3 )//阶梯结算日数组
                {
                    Len.w = (( DataType== eData) ? (MAX_YEAR_BILL*3) : (2+(MAX_YEAR_BILL)*8) );
                }
			}
			#endif
			else if( 	(OI.w == 0x4007)	//LCD参数 
					|| 	(OI.w == 0x400C) 	//时区时段数
					|| 	(OI.w == 0x4011)	//公共假日表
					|| 	(OI.w == 0x4014)	//当前套时区表
					|| 	(OI.w == 0x4015) 	//备用套时区表
                    #if(PREPAY_MODE == PREPAY_LOCAL)  
                    || 	(OI.w == 0x4018)    //当前套费率电价
                    || 	(OI.w == 0x4019)    //备用套费率电价
                    || 	(OI.w == 0x401e)    //0x401e:报警金额限值
					|| 	(OI.w == 0x401f)    //0x401f:其他金额限值 
                    #endif//#if(PREPAY_MODE == PREPAY_LOCAL)  
					|| 	(OI.w == 0x4116)	//结算日
					|| 	(OI.w == 0x4030) )	//电压合格率
			{
				if( OI.w == 0x4011 )//公共假日表
				{
					TagPara.Array_Struct_Num = FPara1->TimeZoneSegPara.HolidayNum;
					if( ClassIndex > MAX_HOLIDAY )//支持先改公共假日表后更改公共假日数的编程记录
					{
						return 0x8000;
					}
				}
				else if( (OI.w == 0x4014) || (OI.w == 0x4015) )//时区表
				{
					//源数据按最大长度传，加tag按实际长度加tag
					if( DataType == eData )
					{
                        TagPara.Array_Struct_Num = MAX_TIME_AREA;
					}
					else
					{
                        TagPara.Array_Struct_Num = FPara1->TimeZoneSegPara.TimeZoneNum;
					}
					
					if( ClassIndex > MAX_TIME_AREA )
					{
						return 0x8000;
					}
				}
                #if(PREPAY_MODE == PREPAY_LOCAL)
		        else if( (OI.w == 0x4018) || (OI.w == 0x4019) )//当前套费率电价、备用套费率电价
				{
					if( DataType == eData )
					{
						TagPara.Array_Struct_Num = MAX_RATIO;
					}
					else
					{
						TagPara.Array_Struct_Num = FPara1->TimeZoneSegPara.Ratio;
					}
                    
					if( ClassIndex > FPara1->TimeZoneSegPara.Ratio )
					{
						return 0x8000;
					}
				}
                #endif//#if(PREPAY_MODE == PREPAY_LOCAL)
				else if( OI.w == 0x4116 )//结算日
				{
					TagPara.Array_Struct_Num = MAX_MON_CLOSING_NUM;
					if( ClassIndex > MAX_MON_CLOSING_NUM )
					{
						return 0x8000;
					}
				}
				else
				{
                    if( ClassIndex > ParaMeterObj[Sch].Type2[1] )
                    {
                        return 0x8000;
                    }
				}
			
				Tag = ParaMeterObj[Sch].Type2;
				Tag +=GetTagOffsetAddr( ClassIndex, Tag);
				Len.w = GetTypeLen( DataType, Tag );
				return Len.w;
			}
			else//普通数据
			{
				if( ClassIndex != 0 )
				{
					return 0x8000;
				}
				Tag = ParaMeterObj[Sch].Type2;
				Len.w = GetTypeLen( DataType, Tag) ;
				return Len.w;		
			}		
			break;
		case 4:
			if( OI.w == 0x4800 )//Len属性4
			{
				TagPara.Array_Struct_Num = NumofNILM;
				if( ClassIndex > TagPara.Array_Struct_Num)
				{
					return 0x8000;
				}
				
				Tag = (BYTE *)&A_4800_4_ApplFrzData;
				Tag +=GetTagOffsetAddr( ClassIndex, Tag);
				Len.w = GetTypeLen4804( eTagData, Tag ,(BYTE*)&tApplFrzDataFLASH[0]);
			}
			break;
		case 5:
			if( OI.w == 0x4000 )//广播校时参数
			{
				if( ClassIndex > T_TimeBroadCastPara[1] )
				{
					return 0x8000;
				}
				
				Tag = T_TimeBroadCastPara;
				Tag +=GetTagOffsetAddr( ClassIndex, Tag);
				Len.w = GetTypeLen( DataType, Tag );
				return Len.w;
			}
			else
			{
				return 0x8000;
			}
			break;

		default:		
			return 0x8000;
	}

	return Len.w;
}

#endif//#if ( SEL_DLT698_2016_FUNC == YES)

