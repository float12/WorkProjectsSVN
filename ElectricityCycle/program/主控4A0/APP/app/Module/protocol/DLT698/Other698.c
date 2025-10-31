//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部
//创建人	姜文浩
//创建日期	2016.01.09
//描述		DL/T 698.45面向对象协议电能读取C文件
//修改记录
//----------------------------------------------------------------------
#include "AllHead.h"
#include "Dlt698_45.h"
#if ( SEL_DLT698_2016_FUNC == YES)
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
//这个数据比较长！！！！！！
//#define MAX_OTHER_BUF_LENTH         (MAX_LCD_KEY_ITEM*10+30)//液晶按键显示为最大BUF长度
#define MAX_OTHER_BUF_LENTH         (99*10+30)//液晶按键显示为最大BUF长度

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

typedef struct TOtherObj_t
{
	WORD				OI;					//698对象标识：OI对象标识
	BYTE         		Class;				//归属类
	BYTE         		MAXClassAttribute;	//最大属性
}TOtherObj;


//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------

//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
//端口描述符 ascii码
const char PortDescript0[] 	= "RS4851";
const char PortDescript1[] 	= "INFRA";
const char PortDescript2[]	= "HPLC+HRF";
const char PortDescript3[] 	= "RS4852";
const char PortDescript4[] 	= "RS485";
const char PortDescript5[] 	= "CAN";
const char Relayscript[]   	= "RELAY";
//遥控属性2	
static const BYTE T_RemoteControl[]		= 
{ 
	Structure_698,				2,
	Double_long_unsigned_698,	4,
	Long_unsigned_698,			2,
};

//安全模式参数3
static const BYTE T_SafeMode3[]		= 
{ 
	Array_698,			0xff,
	Structure_698,		2,
	OI_698,				2,
	Long_unsigned_698,	2,
};

//RS485属性2
static const BYTE T_RS485[]		= 
{ 
	Array_698,			0xff,
	Structure_698,		3,
	Visible_string_698,	0xff,
	COMDCB_698,			5,
	Enum_698,			1,
};
//载波属性
static const BYTE T_CR[]		= 
{ 
	Array_698,			0xff,
	Structure_698,		3,
	Visible_string_698,	0xff,
	COMDCB_698,			5,
	NULL_698,			1,
};
//CAN属性2
static const BYTE T_CAN[]		=
{
	Array_698,			1,
	Structure_698,		2,
	Visible_string_698,	0xff,
	COMDCB_698,			5,
};

//红外属性2
static const BYTE T_IR[]		= 
{ 
	Array_698,			1,
	Structure_698,		2,
	Visible_string_698,	0xff,
	COMDCB_698,			5,
};
//继电器输出
static const BYTE T_RelayOut[]		= 
{ 
	Array_698,			1,
	Structure_698,		4,
	Visible_string_698,	0xff,
	Enum_698,			1,
	Enum_698,			1,
	Enum_698,			1,
};
//多功能端子
static const BYTE T_MultiFunPort[]		= 
{ 
	Array_698,			1,
	Enum_698,			1,
};
//遥信属性2
static const BYTE T_SwitchIn_2[]	=
{
	Array_698,		1,
	Structure_698,		2,
	Unsigned_698,		1,
	Unsigned_698,		1,
};
//遥信属性4
static const BYTE T_SwitchIn_4[]	=
{
	Structure_698,		2,
	Bit_string_698,		8,
	Bit_string_698,		8,
};
//显示参数
static const BYTE T_Display[]		= 
{ 
	Structure_698,	2,
	Unsigned_698,	1,
	Unsigned_698,	1,
};

//ESAM计数器
static const BYTE T_NowCount[]		= 
{ 
	Structure_698,				    3,
	Double_long_unsigned_698,		4,
	Double_long_unsigned_698,		4,
	Double_long_unsigned_698,		4,
};

//ESAM终端会话门限
static const BYTE T_TerminalLimit[]		= 
{ 
	Structure_698,				    2,
	Double_long_unsigned_698,		4,
	Double_long_unsigned_698,		4,
};
static const BYTE T_TransfeStatus[] = // 传输状态字
{
	Bit_string_698,		0xFF,
};
static const BYTE T_DownloadID[] = // 下载方标识
{
	Visible_string_698,	0xFF,
};
static const BYTE T_BlockSize[] = // 传输块尺寸
{
	Long_unsigned_698,	2,
};
static const BYTE T_UpdatTime[] = // 升级时间
{
	DateTime_S_698,	7,
};

// const BYTE T_BluetoothModule[] = //蓝牙模块 属性2
// {
// 	Structure_698,				5,
// 	Unsigned_698,				1,		//模块序号

// 	Structure_698,				2,		//BluetoothInfo		T_BluetoothInfo
// 	Visible_string_698,			0x09,	//蓝牙描述符
// 	Octet_string_698,			0x06,	//蓝牙MAC地址

// 	Visible_string_698,			0x06,	//配对密码
// 	Enum_698,					1,		//配对模式
// 	Octet_string_698,			20,		//蓝牙序列号
// };

// const BYTE T_BluetoothDevice[] = //蓝牙属性4， 蓝牙从设备 
// {
// 	Structure_698,				2,
// 	Octet_string_698,			0x06,	//蓝牙MAC地址
// 	Octet_string_698,			0x10,	//蓝牙资产号
// };

// const BYTE T_BluetoothWork[] =	//蓝牙属性6  工作参数
// {
// 	Structure_698,			4,		
// 	OAD_698, 				4,		//端口号
// 	Unsigned_698, 			4,		//发射功率档
// 	Long_unsigned_698,		2,		//广播间隔
// 	Long_unsigned_698,		2,		//扫描间隔
// };

//增加数据类型请查看源数据和加TAG是否支持
static const TOtherObj OtherObj[] =
{
	{  0x8000, 	eclass8,	5	},	//遥控
	{  0x8001, 	eclass8,	2	},	//保电
	{  0xF100, 	eclass21,	21	},	//ESAM
	{  0xF101, 	eclass8,	3	},	//安全模式参数
	{  0xF201, 	eclass22,	2	},	//RS485
	{  0xF202, 	eclass22,	2	},	//红外
	{  0xF203, 	eclass22,	4	},	//遥信
	{  0xF205, 	eclass22,	2	},	//继电器输出
	{  0xF207, 	eclass22,	2	},	//多功能端子
	{  0xF209, 	eclass22,	2	},	//载波/微功率无线接口
	// {  0xF20B, 	eclass22,	7	},	//蓝牙模块
	{  0xF221, 	eclass22,	7	},	//CAN
	{  0xF300, 	eclass17,	4	},	//自动轮显
	{  0xF301, 	eclass17,	4	},	//按键轮显
	{  0xF001, 	eclass18,	7	},	//文件分块传输
};


//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------


//-----------------------------------------------
//				函数定义
//-----------------------------------------------

//--------------------------------------------------
//功能描述:  根据表格查找参数OAD索引
//         
//参数:      OI[in]：OI
//         
//返回值:    参数索引
//         
//备注内容:  无
//--------------------------------------------------
static BYTE SearchOtherOAD( WORD OI )
{
	BYTE i,Num;//数组个数不能超过255

	Num = (sizeof(OtherObj)/sizeof(TOtherObj));
	if( Num >= 0x80 )//避免死循环
	{
		return 0x80;
	}
	
	for(i=0; i<Num; i++)
	{
		if( OI == OtherObj[i].OI )
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
//功能描述: 获取参数（8000、8001、F101）（class8）属性数据（不带Tag）
//         
//参数:
//			Dot[in]：	小数点 （未用到）    
//          *pOAD[in]：	OAD         
//          Sch[in]：   OtherObj中的事件索引      
//          *pBuf[out]：输出数据
//         
//返回值:   数据长度
//         
//备注内容: OI 	8000 属性2 属性4 属性5
//				8001 属性2
//				F101 属性2 属性3
//--------------------------------------------------
static WORD GetOtherClass8Data( BYTE Dot, BYTE *pOAD, BYTE Sch, BYTE *pBuf )
{
	BYTE ClassAttribute,ClassIndex,i,Num;
	TTwoByteUnion OI;	
	WORD Result,Lenth;
	BYTE *pBufBak = pBuf;
	const BYTE *Tag;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);
	ClassIndex = pOAD[3];
	Lenth = 0;
	
	switch( OI.w )
	{
	case 0x8000://远程控制
		switch( ClassAttribute )
		{
		case 0x02:
			if( ClassIndex > 2 )
			{
				return 0x8000;
			}
			
			Tag = T_RemoteControl;
			Tag += GetTagOffsetAddr( ClassIndex, Tag);
			Lenth = GetTypeLen( eData, Tag );
			if( Lenth == 0x8000 )
			{
                return 0x8000;
			}
			
			i = (ClassIndex == 0) ? 0 : (ClassIndex-1);
			Num = (ClassIndex == 0) ? 2 : 1;
			Num += i;

			for( ; i<Num; i++ )
			{
				Result = ReadRelayPara(i,pBuf);
				if(Result == FALSE)
				{
					return 0x8000;
				}

				if( i == 0 )
				{
					pBuf += 4;
				}
				else
				{
					pBuf += 2;
				}
			}
			break;
		case 0x04:
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}

			if( api_GetRelayStatus(3) == FALSE )
			{
				*(pBuf++) = 0x00;
			}
			else //远程报警
			{
				*(pBuf++) = 0x01;
			}
			break;
		case 0x05:
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			//命令合闸
			#if(PREPAY_MODE == PREPAY_LOCAL)
			if( api_GetRelayStatus(7) == 0 )
			#else
			if( api_GetRelayStatus(0) == 0 )
			#endif
			{
				*(pBuf++) = 0x00;
			}
			else //命令跳闸
			{
				*(pBuf++) = 0x01;
			}
			break;
		default:
			return 0x8000;
		}
		break;
	case 0x8001://保电
		if( ClassAttribute == 0x02 )
		{
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			
			if( api_GetRelayStatus(2) == FALSE )
			{
				*(pBuf++) = 0x00;
			}
			else //保电状态
			{
				*(pBuf++) = 0x01;
			}
		}
		else
		{
			return 0x8000;
		}
		break;
	#if(SEL_ESAM_TYPE != 0)
	case 0xF101://安全模式参数
		if( ClassAttribute == 0x02 )
		{
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			*(pBuf++) = 0x01;
		}
		else if( ClassAttribute == 0x03 )
		{
			if( ClassIndex > SafeModePara.Num )
			{
				return 0x8000;
			}
			
			i = (ClassIndex == 0) ? 0 : (ClassIndex-1);
			Num = (ClassIndex == 0) ? SafeModePara.Num : 1;
			TagPara.Array_Struct_Num = Num;
			Num += i;

			for( ; i<Num; i++ )
			{
				*(pBuf++) = SafeModePara.Sub[i].OI/0x100;//OI的高字节 OI先传高字节，
				*(pBuf++) = SafeModePara.Sub[i].OI&0xff;//OI的低字节
				*(pBuf++) = SafeModePara.Sub[i].wSafeMode&0xff;//safemode的低字节  安全模式先传低字节，符合EEPROM中存储规律，jwh那边根据协议倒序下 wlk
				*(pBuf++) = SafeModePara.Sub[i].wSafeMode/0x100;//safemode的高字节
			}
		}
		else
		{
			return 0x8000;
		}
		break;
	#endif
	default:
		break;
	}

	return  (WORD)(pBuf-pBufBak);
}

//--------------------------------------------------
//功能描述: 对参数（8000、8001、F101）（class8）数据添加Tag
//         
//参数:
//         	*pOAD[in]：		OAD
//			Sch[in]：		OtherObj中的事件索引         
//          *InBuf[in]：	需要添加tag的数据         
//          OutBufLen[in]：	申请的缓冲长度         
//          *OutBuf[in]：	输出数据
//         
//返回值:   数据长度
//         
//备注内容: OI 	8000 属性2 属性4 属性5
//				8001 属性2
//				F101 属性2 属性3
//--------------------------------------------------
static WORD AddTagOtherClass8Data(BYTE *pOAD, BYTE Sch, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE ClassAttribute,ClassIndex;
	const BYTE *Tag;
	TTwoByteUnion OI;
	WORD Result,Lenth;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);
	ClassIndex = pOAD[3];
	Lenth = 0;

	switch( OI.w )
	{
		case 0x8000:
			if( ClassAttribute == 0x02 )
			{
				if( ClassIndex > 2 )
				{
					return 0x8000;
				}
				
				Tag = T_RemoteControl;
				Tag += GetTagOffsetAddr(ClassIndex, Tag);
				Lenth = GetTypeLen( eTagData, Tag );
    			if( Lenth == 0x8000 )
    			{
                    return 0x8000;
    			}
			}
			else if( (ClassAttribute==0x04) || (ClassAttribute==0x05) )
			{
			    if( ClassIndex != 0 )
				{
					return 0x8000;
				}
				
				Tag = T_BitString8;
				Lenth = GetTypeLen( eTagData, T_BitString8 );	
    		    if( Lenth == 0x8000 )
    			{
                    return 0x8000;
    			}
			}
			else
			{
				return 0x8000;
			}
			break;
		case 0x8001:
			if( ClassAttribute == 0x02 )
			{
				if( ClassIndex != 0 )
				{
					return 0x8000;
				}
				
				Tag = T_Enum;
				Lenth = GetTypeLen( eTagData, T_Enum );	
    			if( Lenth == 0x8000 )
    			{
                    return 0x8000;
    			}
			}
			else
			{
				return 0x8000;
			}			
			break;
		case 0xf101:
			if( ClassAttribute == 0x02 )
			{
				if( ClassIndex != 0 )
				{
					return 0x8000;
				}
				
				Tag = T_Enum;
				Lenth = GetTypeLen( eTagData, T_Enum );	
    		    if( Lenth == 0x8000 )
    			{
                    return 0x8000;
    			}
			}
			else if( ClassAttribute == 0x03 )
			{
				if( ClassIndex > SafeModePara.Num )
				{
					return 0x8000;
				}
				if(SafeModePara.Num == 0)//浙江用户要求为空时返回0100
				{
					OutBuf[0]=0x01;
					OutBuf[1]=0x00;
					return 2;
				}
				
				TagPara.Array_Struct_Num = (ClassIndex == 0) ? SafeModePara.Num : 1;
				Tag = T_SafeMode3;
				Tag += GetTagOffsetAddr(ClassIndex, Tag);
				Lenth = GetTypeLen( eTagData, Tag );
    			if( Lenth == 0x8000 )
    			{
                    return 0x8000;
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

//--------------------------------------------------
//功能描述: 获取参数（8000、8001、F101）（class8）数据长度
//         
//参数:     
//          DataType[in]：eData/eTagData
//          *pOAD[in]     OAD    
//         
//返回值:   返回数据长度
//         
//备注内容: OI 	8000 属性2 属性4 属性5
//				8001 属性2
//				F101 属性2 属性3
//--------------------------------------------------
static WORD GetRequestOtherClass8Len( BYTE DataType, BYTE *pOAD)
{
	BYTE ClassAttribute,ClassIndex;
	const BYTE *Tag;
	TTwoByteUnion OI;
	WORD Lenth;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);
	ClassIndex = pOAD[3];
	Lenth = 0;

	switch( OI.w )
	{
		case 0x8000:
			if( ClassAttribute == 0x02 )
			{
				if( ClassIndex > 2 )
				{
					return 0x8000;
				}
				
				Tag = T_RemoteControl;
				Tag += GetTagOffsetAddr( ClassIndex, Tag);
				Lenth = GetTypeLen( DataType, Tag );
    		    if( Lenth == 0x8000 )
    			{
                    return 0x8000;
    			}
			}
			else if( (ClassAttribute==0x04) || (ClassAttribute==0x05) )
			{
				if( ClassIndex != 0 )
				{
					return 0x8000;
				}
				Lenth = GetTypeLen( DataType, T_BitString8 );	
    			if( Lenth == 0x8000 )
    			{
                    return 0x8000;
    			}
			}
			else
			{
				return 0x8000;
			}
			break;
		case 0x8001:
			if( ClassAttribute == 0x02 )
			{
				if( ClassIndex != 0 )
				{
					return 0x8000;
				}
				
				Lenth = GetTypeLen( DataType, T_Enum );	
    			if( Lenth == 0x8000 )
    			{
                    return 0x8000;
    			}
			}
			else
			{
				return 0x8000;
			}
			break;
		case 0xF101:
			if( ClassAttribute == 0x02 )
			{
				if( ClassIndex != 0 )
				{
					return 0x8000;
				}
				
				Lenth = GetTypeLen( DataType, T_Enum );
    			if( Lenth == 0x8000 )
    			{
                    return 0x8000;
    			}
			}
			else if( ClassAttribute == 0x03 )
			{
				if( ClassIndex > SafeModePara.Num )
				{
					return 0x8000;
				}
				
				TagPara.Array_Struct_Num = (ClassIndex == 0) ? SafeModePara.Num : 1;
				Tag = T_SafeMode3;
				Tag += GetTagOffsetAddr( ClassIndex, Tag);
				Lenth = GetTypeLen( DataType, Tag );
    			if( Lenth == 0x8000 )
    			{
                    return 0x8000;
    			}
			}
			else
			{
				return 0x8000;
			}
			break;
		default:
			break;
	}
	
	return Lenth;
}

//--------------------------------------------------
//功能描述: 获取ESAM参数（F100）（class21）属性数据（不带Tag）
//         
//参数:
//			Dot[in]：	小数点 （未用到）    
//          *pOAD[in]：	OAD         
//          Sch[in]：   OtherObj中的事件索引      
//          *pBuf[out]：输出数据
//         
//返回值:   数据长度
//         
//备注内容: 
//--------------------------------------------------
static WORD GetOtherClass21Data( BYTE Ch, BYTE Dot, BYTE *pOAD, BYTE Sch, BYTE *pBuf )
{	
	BYTE ClassAttribute,ClassIndex,i,Value,Index;
	BYTE Tag[] = {0,0,2,1,4,5,5,6,8,0x11,0x12,0x13,0x14};
	WORD Result;
	TTwoByteUnion OI,DataLen;
	TFourByteUnion ConnectValidTime;
	BYTE *pBufBak = pBuf;		
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1F);//bit0…bit4编码表示对象属性编号；
	ClassIndex = pOAD[3];
	
	switch( ClassAttribute )
	{
		case 0x02: 	//获取ESAM序列号
		case 0x03: 	//获取ESAM版本号
		case 0x04: 	//获取对称密钥版本
		case 0x05: 	//获取会话时效门限
	    case 0x06: 	//获取会话剩余时间
		case 0x07: 	//获取计数器
		case 17:  	//获取本地计算指令使用权限---不能随意改变值-jwh
		case 18: 	//获取终端地址
		case 19: 	//获取终端广播计数器
		case 20: 	//获取终端与电表会话计数器
		case 21: 	//获取终端会话门限
			if( ClassAttribute == 7 )//主站计数器
			{
				if( ClassIndex > 3 )
				{
					return 0x8000;
				}
			}
			else if( ClassAttribute == 21 )//终端会话门限
			{
				if( ClassIndex > 2 )
				{
					return 0x8000;
				}
			}
			else
			{
				if( ClassIndex != 0 )
				{
					return 0x8000;
				}
			}
			
			Index = ClassAttribute;
			if( Index >= 17 )//不能随意改变值-jwh
			{
				Index -= 9;
			}
			
			DataLen.w = api_GetEsamInfo( Tag[Index], pBuf );
			if( DataLen.w > (MAX_OTHER_BUF_LENTH - 2) )
			{
				return 0x8000;
			}
			
			if( (ClassAttribute == 5) || (ClassAttribute == 6))//会话时效门限、会话剩余时间
			{
			    if( (api_GetSysStatus( eSYS_STATUS_ID_698MASTER_AUTH ) == FALSE) && (ClassAttribute == 6) )//未建立应用链接时 会话剩余时间为0
    			{
    				memset( pBuf, 0x00, 4 );
    			}
    			else
    			{
    				//此处使用exchange是因为数据标识位doublelong，加tag时会进行倒序，为防止数据不变在数据源处进行倒序
    				lib_ExchangeData( pBuf, pBuf+4*(ClassAttribute-5), 4 );
    			}
				pBuf += 4;
			}
			else if( (ClassAttribute == 19) || (ClassAttribute == 20) )//终端广播计数器、终端与电表会话计数器
			{
				//此处使用exchange是因为数据标识位doublelong，加tag时会进行倒序，为防止数据不变在数据源处进行倒序
				lib_InverseData( pBuf, 4 );
				pBuf += 4;
			}
			else if( ClassAttribute == 7 )//主站会话计数器
			{
				if( ClassIndex == 0 )
				{
					for( i=0; i < 3; i++ )
					{
						lib_InverseData( pBuf, 4 );
						pBuf += 4;
					}
					
				}
				else
				{
					lib_ExchangeData( pBuf, pBuf+4*(ClassIndex-1), 4 );
					pBuf += 4;
				}				
			}
			else if( ClassAttribute == 21 )//终端会话门限
			{
				if( api_GetSysStatus( eSYS_STATUS_ID_698TERMINAL_AUTH ) == FALSE )//未建立应用链接时 会话剩余时间为0
    			{
    				memset( pBuf+4, 0x00, 4 );
    			}
    			
				if( ClassIndex == 0 )
				{
					for( i=0; i < 2; i++ )
					{
						lib_InverseData( pBuf, 4 );
						pBuf += 4;
					}
				}
				else
				{
					lib_ExchangeData( pBuf, pBuf+4*(ClassIndex-1), 4 );
					pBuf += 4;
				}	
			}
			else
			{
				pBuf += DataLen.w;
			}
			break;
		
		case 0x0E://红外认证时效门限
            if( ClassIndex != 0 )
            {
                return 0x8000;
            }
			
			api_ReadPrePayPara( eIRTime, pBuf );
            pBuf += 4;
            
		    break;
		    
		case 0x0F://红外认证时效剩余时间
            if( ClassIndex != 0 )
            {
                return 0x8000;
            }
			
			if( CURR_COMM_TYPE( Ch ) == COMM_TYPE_TERMINAL )
			{
				ConnectValidTime.l = (APPConnect.TerminalAuthTime / 60);
				Value = (APPConnect.TerminalAuthTime%60);				
			}
			else
			{
				ConnectValidTime.l = (APPConnect.IRAuthTime / 60);
				Value = (APPConnect.IRAuthTime%60);
			}
            if( Value != 0 )//如果有余数则加上1分钟
            {
                ConnectValidTime.l += 1;
            }
            
            memcpy( pBuf, ConnectValidTime.b, 4 );//因为是double long数据类型 不进行倒序 在加TAG时会进行倒序

            pBuf += 4;
		    break;
		default:
			return 0x8000;
	}

	return  (WORD)(pBuf-pBufBak);
}

//--------------------------------------------------
//功能描述: 对ESAM参数（F100）（class21）属性数据添加Tag
//         
//参数:
//         	*pOAD[in]：		OAD
//			Sch[in]：		OtherObj中的事件索引         
//          *InBuf[in]：	需要添加tag的数据         
//          OutBufLen[in]：	申请的缓冲长度         
//          *OutBuf[in]：	输出数据
//         
//返回值:   数据长度
//         
//备注内容: 
//--------------------------------------------------
static WORD AddTagOtherClass21Data(BYTE *pOAD, BYTE Sch, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE ClassAttribute,ClassIndex;
	const BYTE *Tag;
	TTwoByteUnion OI;
	WORD Result,Lenth;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1F);//bit0…bit4编码表示对象属性编号；
	ClassIndex = pOAD[3];
	Lenth = 0;

	switch( ClassAttribute )
	{
		case 0x02: //获取ESAM序列号
		case 0x03: //获取ESAM版本号
		case 0x04: //获取对称密钥版本
		case 17:  //本地计算指令使用权限
		case 18:  //终端地址
		case 0x05: //获取会话时效门限
		case 0x06: //获取会话剩余时间
		case 0x0E: //获取红外认证时效
		case 0x0F: //获取红外认证时效剩余时间
		case 19:  //获取终端广播计数器
		case 20:  //获取终端与电表会话计数器
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			
			if( ClassAttribute == 2 )//esam序列号
			{
				Tag = T_OctetString8;
			}
			else if( ClassAttribute == 3 )//esam版本号
			{
				Tag = T_OctetString5;
			}
			else if( ClassAttribute == 4 )//对称密钥版本
			{
				Tag = T_OctetString16;
			}
			else if( ClassAttribute == 17 )//本地计算指令使用权限
			{
				Tag = T_Unsigned;
			}
			else if( ClassAttribute == 18 )//终端地址
			{
				Tag = T_OctetString8;
			}
			else//会话时效门限、会话剩余时间、红外认证时效、红外认证时效剩余时间、终端广播计数器、终端与电表会话计数器
			{
				Tag = T_UNDoubleLong;
			}

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

			break;
			
		case 0x07: //获取计数器
		case 21: //获取终端会话门限
			if( ClassAttribute == 0x07 )
			{
				if( ClassIndex > T_NowCount[1] )
				{
					return 0x8000;
				}
				Tag = T_NowCount;
			}
			else
			{
				if( ClassIndex > T_TerminalLimit[1] )
				{
					return 0x8000;
				}
				Tag = T_TerminalLimit;
			}

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
			break;

		default:
			return 0x8000;
	}	
	
	return Lenth;
}

//--------------------------------------------------
//功能描述: 获取ESAM参数（F100）（class21）数据长度
//         
//参数:     
//          DataType[in]：eData/eTagData
//          *pOAD[in]     OAD    
//         
//返回值:   返回数据长度
//         
//备注内容: 
//--------------------------------------------------
static WORD GetRequestOtherClass21Len( BYTE DataType, BYTE *pOAD)
{
	BYTE ClassAttribute,ClassIndex;
	const BYTE *Tag;
	TTwoByteUnion OI;
	WORD Lenth;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1F);//bit0…bit4编码表示对象属性编号；
	ClassIndex = pOAD[3];
	Lenth = 0;

	switch( ClassAttribute )
	{
		case 0x02: //获取ESAM序列号
		case 0x03: //获取ESAM版本号
		case 0x04: //获取对称密钥版本
		case 17:  //本地计算指令使用权限
		case 18:  //终端地址
		case 0x05: //获取会话时效门限
		case 0x06: //获取会话剩余时间
		case 0x0E: //获取红外认证时效
		case 0x0F: //获取红外认证时效剩余时间
		case 19:  //获取终端广播计数器
		case 20:  //获取终端与电表会话计数器
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			
			if( ClassAttribute == 2 )//esam序列号
			{
				Tag = T_OctetString8;
			}
			else if( ClassAttribute == 3 )//esam版本号
			{
				Tag = T_OctetString5;
			}
			else if( ClassAttribute == 4 )//对称密钥版本
			{
				Tag = T_OctetString16;
			}
			else if( ClassAttribute == 17 )//本地计算指令使用权限
			{
				Tag = T_Unsigned;
			}
			else if( ClassAttribute == 18 )//终端地址
			{
				Tag = T_OctetString8;
			}
			else//会话时效门限、会话剩余时间、红外认证时效、红外认证时效剩余时间、终端广播计数器、终端与电表会话计数器
			{
				Tag = T_UNDoubleLong;
			}

			Lenth = GetTypeLen( DataType, Tag );
			
			break;
			
		case 0x07: //获取计数器
		case 21: //获取终端会话门限
			if( ClassAttribute == 0x07 )
			{
				if( ClassIndex > T_NowCount[1] )
				{
					return 0x8000;
				}
				Tag = T_NowCount;
			}
			else
			{
				if( ClassIndex > T_TerminalLimit[1] )
				{
					return 0x8000;
				}
				Tag = T_TerminalLimit;
			}
			
			Tag +=GetTagOffsetAddr( ClassIndex, Tag);
			Lenth = GetTypeLen( DataType, Tag );
			break;
		
		default:
			return 0x8000;
	}	
	
	return Lenth;
}

//--------------------------------------------------
//功能描述: 获取输入输出设备（class22）属性数据（不带Tag）
//         
//参数:
//			Dot[in]：	小数点 （未用到）    
//          *pOAD[in]：	OAD         
//          Sch[in]：   OtherObj中的事件索引      
//          *pBuf[out]：输出数据
//         
//返回值:   数据长度
//         
//备注内容: 
//--------------------------------------------------
static WORD GetOtherClass22Data( BYTE Dot, BYTE *pOAD, BYTE Sch, BYTE *pBuf )
{
	BYTE ClassAttribute,ClassIndex,i,Num,Flag,TempCommPara;
	TTwoByteUnion OI;	
	WORD Result;
	BYTE *pBufBak = pBuf;
//	T_ble_para pPara_inf;
	// TSafeMem_dev_work dev_work;

	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);//bit0…bit4编码表示对象属性编号；
	ClassIndex = pOAD[3];
	Flag = 0;
	
	switch( ClassAttribute )
	{
		case 0x02: //设备对象列表
			switch( OI.w )
			{
				case 0xF201://RS485
					if( ClassIndex > 2 )
					{
						return 0x8000;
					}

					for(i=0;i<MAX_COM_CHANNEL_NUM;i++)
					{
						if((SerialMap[i].SerialType==eRS485_I) /*|| (SerialMap[i].SerialType == eRS485_II)*/ )
						{
							Flag++;
						}
					}
					if( Flag == 0 )
					{
						return 0x8000;
					}
					
					TagPara.Array_Struct_Num = Flag;
					#if( MAX_PHASE != 1)
					TagPara.Lenth.w = (sizeof(PortDescript4)-1);
					#else
                    TagPara.Lenth.w = (sizeof(PortDescript4)-1);
					#endif
					i = (ClassIndex == 0) ? 0 : (ClassIndex-1);
					Num = (ClassIndex == 0) ? Flag : 1;
					Num += i;

					for( ; i < Num; i++ )
					{
						if( i == 0 )
						{
						    #if( MAX_PHASE != 1)
							memcpy(pBuf,PortDescript4,(sizeof(PortDescript4)-1));//端口描述符
							pBuf += (sizeof(PortDescript4)-1);
							#else
							memcpy(pBuf,PortDescript4,(sizeof(PortDescript4)-1));//端口描述符
							pBuf += (sizeof(PortDescript4)-1);
							#endif
							TempCommPara = FPara2->CommPara.I485;//端口参数
						}
						else
						{
							memcpy(pBuf,PortDescript3,(sizeof(PortDescript3)-1));//端口描述符
							pBuf += (sizeof(PortDescript3)-1);
							TempCommPara = FPara2->CommPara.II485;//端口参数
						}
						
						*(pBuf++) = (TempCommPara & 0x0F);//波特率
						*(pBuf++) = ((TempCommPara>>5) & 0x03);//校验位
						*(pBuf++) = 8;//数据位
						*(pBuf++) = (((TempCommPara>>4) & 0x01)+1);//停止位
						*(pBuf++) = 0;//流控
						*(pBuf++) = 0;//端口功能
					}
					break;	
				case 0xF202://IR
					if( ClassIndex > 1 )
					{
						return 0x8000;
					}
					
					TagPara.Lenth.w = (sizeof(PortDescript1)-1);
					memcpy(pBuf,PortDescript1,(sizeof(PortDescript1)-1));//端口描述符
					pBuf += (sizeof(PortDescript1)-1);

					//红外全部为固定值，不需要取ComPara参数，直接配置
					*(pBuf++) = 2;//波特率: 1200bps
					*(pBuf++) = 2;//校验位: 偶校验
					*(pBuf++) = 8;//数据位: 8位数据位
					*(pBuf++) = 1;//停止位: 1位停止位
					*(pBuf++) = 0;//流控:   无
					break;
				#if(SEL_EVENT_DI_CHANGE == YES)	
				case 0xF203:
					*(pBuf++) = api_ReadDIStatus();//遥信状态
					*(pBuf++) = api_ReadDIChange();//遥信变位状态					
					break;
				#endif
				case 0xF205://继电器输出
					if( ClassIndex > 1 )
					{
						return 0x8000;
					}
					
					TagPara.Lenth.w = (sizeof(Relayscript)-1);
					memcpy(pBuf,Relayscript,(sizeof(Relayscript)-1));//端口描述符
					pBuf += (sizeof(Relayscript)-1);
					//当前状态  enum{未输出（0），输出（1）}
					if( api_GetRelayStatus(1) == 0 )
					{
						*(pBuf++) = 0;
					}
					else
					{
						*(pBuf++) = 1;
					}

					Result = ReadRelayPara(3,pBuf);//开关属性  enum{脉冲式（0），保持式（1）}，
					if(Result == FALSE)
					{
						return 0x8000;
					}
					pBuf += 1;

					*(pBuf++) = 0;//接线状态  enum{接入（0），未接入（1) }
					break;
				case 0xF207://多功能端子
					if( ClassIndex > 1 )
					{
						return 0x8000;
					}
					
					*(pBuf++)= GlobalVariable.g_byMultiFunPortType;
					break;
				case 0xF209://载波、微功率无线接口
					if( ClassIndex > 1 )
					{
						return 0x8000;
					}

					for(i=0;i<MAX_COM_CHANNEL_NUM;i++)
					{
						if( SerialMap[i].SerialType == eCR)
						{
							Flag++;
						}
					}
					
					if(Flag != 1)
					{
						return 0x8000;
					}
					memcpy(pBuf,PortDescript2,(sizeof(PortDescript2)-1));//端口描述符
					pBuf += (sizeof(PortDescript2)-1);
					
					TagPara.Lenth.w = sizeof(PortDescript2);
					//*(pBuf++) = 0x00;

					TempCommPara = FPara2->CommPara.ComModule;//端口参数
					*(pBuf++) = (TempCommPara & 0x0F);//波特率
					*(pBuf++) = ((TempCommPara>>5) & 0x03);//校验位
					*(pBuf++) = 8;//数据位
					*(pBuf++) = (((TempCommPara>>4) & 0x01)+1);//停止位
					*(pBuf++) = 0;//流控

					*(pBuf++) = NULL;//版本信息为空
					break;
//				case 0xF20B:	//蓝牙 属性2 蓝牙模块
//					i = api_GetBlueToothModule698(pBuf);
//					pBuf += i;
				case 0xF221:	//CAN总线 属性2
					if( ClassIndex > 2 )
					{
						return 0x8000;
					}
					TagPara.Lenth.w = (sizeof(PortDescript5)-1);
					memcpy(pBuf,PortDescript5,(sizeof(PortDescript5)-1));//端口描述符
					pBuf += (sizeof(PortDescript5)-1);
					TempCommPara = FPara2->CommPara.CanBaud;//端口参数
					*(pBuf++) = TempCommPara ;//波特率	
					*(pBuf++) = 0;//校验位
					*(pBuf++) = 0;//数据位
					*(pBuf++) = 0;//停止位
					*(pBuf++) = 0;//流控	
					break;
				default:
					return 0x8000;
			}
			break;

		case 3:
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			
			if( OI.w == 0xf201 )
			{
				for(i=0;i<MAX_COM_CHANNEL_NUM;i++)
				{
					if((SerialMap[i].SerialType==eRS485_I) /*|| (SerialMap[i].SerialType == eRS485_II)*/ )
					{
						Flag++;
					}
				}

				*(pBuf++) = Flag;
			}
			else if( OI.w == 0xf202 )
			{
				*(pBuf++) = 1;
			}
			else if( OI.w == 0xf205 )
			{
				*(pBuf++) = 1;
			}
			else if( OI.w == 0xf207 )
			{
				*(pBuf++) = 1;
			}
			else
			{
				for(i=0;i<MAX_COM_CHANNEL_NUM;i++)
				{
					if( SerialMap[i].SerialType == eCR)
					{
						Flag++;
					}
				}
				*(pBuf++) = Flag;
			}
			break;
		case 4:
			// if(OI.w == 0xf20B)	//蓝牙从设备
			// {
			// 	api_VReadSafeMem(GET_SAFE_SPACE_ADDR(SafeMem_dev_work),sizeof(TSafeMem_dev_work), (BYTE *)&dev_work);
			// 	memcpy(pBuf,dev_work.tDownMessage,sizeof(dev_work.tDownMessage));
			// 	pBuf+=sizeof(dev_work.tDownMessage);
			// }
			// else 
			if(OI.w == 0xf203)	//开关量输入
			{
				api_ReadDIPara(1,pBuf);
				api_ReadDIPara(2,pBuf+1);
				break;
			}
			else
			{
				return 0x8000;
			}
			break;
		case 5:
//			if(OI.w == 0xf20B)	//蓝牙连接信息
//			{
//				*(pBuf++) = 0xF2;//OAD
//				*(pBuf++) = 0x0B;
//				*(pBuf++) = 0x02;
//				*(pBuf++) = 0x01;
//				api_mt_get_ble_para_info(&pPara_inf);
//				for ( i = 0; i < BLE_COM_NUM; i++)
//				{
//					if (pPara_inf.connect_info.status&(BIT0<<i))
//					{
//						if (i<2)
//						{
//							*(pBuf++) = 'N';
//							*(pBuf++) = 'U';
//							*(pBuf++) = 'L';
//							*(pBuf++) = 'L';
//							pBuf+=5;
//						}
//						else
//						{
//							*(pBuf++) = 'B';
//							*(pBuf++) = 'T';
//							*(pBuf++) = '_';
//							*(pBuf++) = 'M';
//							*(pBuf++) = 'E';
//							*(pBuf++) = 'T';
//							*(pBuf++) = 'E';
//							*(pBuf++) = 'R';
//							pBuf++;
//						}
//						memcpy(pBuf,pPara_inf.connect_info.dev[i],6);
//						pBuf+=15;
//					}
//				}
//				
//			}
//			else
			{
				return 0x8000;
			}
			break;
		case 6:
			if (OI.w==0xF221)
			{
				*(pBuf++)=CanBusPara.bSelfNode;
			}
			// else if(OI.w == 0xf20B)	//蓝牙连接信息
			// {
			// 	*(pBuf++) = 0xF2;//OAD
			// 	*(pBuf++) = 0x0B;
			// 	*(pBuf++) = 0x02;
			// 	*(pBuf++) = 0x01;
			// 	api_mt_get_ble_para_info(&pPara_inf);
			// 	lib_ExchangeData(pPara_inf.work_para.adv_interval,pPara_inf.work_para.adv_interval,2);
			// 	lib_ExchangeData(pPara_inf.work_para.scan_interval,pPara_inf.work_para.scan_interval,2);
			// 	memcpy(pBuf,&pPara_inf.work_para,sizeof(pPara_inf.work_para));
			// 	pBuf+=sizeof(pPara_inf.work_para);

			// }
			else
			{
				return 0x8000;
			}
			break;
		case 7:
			if (OI.w==0xF221)
			{
				for ( i = 0; i < CanBusPara.bRxNum; i++)
				{
					*(pBuf++)=CanBusPara.bRxNode[i];
				}
				
			}
			// else if(OI.w == 0xf20B)	//蓝牙 锁具列表
			// {
			// 	for (BYTE ii = 0; ii < 100; ii++)
			// 	{
			// 		*(pBuf++) = ii;
			// 	}
			// }
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
//功能描述:  增加蓝牙TAG
//
//参数: pTag[in]: TAG地址的地址
//		InBuf[in]: 需要加TAG的数据的地址 的地址
//		OutBuf[in]: 输出缓冲
//		OutLen[in]: 输出缓冲的长度
//返回值: 加完TAG的长度
//
//备注: 递归调用
//--------------------------------------------------
// static BYTE s_AddTagBluethCount = 0;
// static WORD AddTagBlueth1(const BYTE **pTag, BYTE **InBuf, BYTE *OutBuf, WORD OutLen)
// {
// 	const BYTE *Tag;
// 	BYTE i, Num;
// 	WORD Lenth = 0, Offset = 0;

// 	s_AddTagBluethCount++;
// 	if(s_AddTagBluethCount > 100)
// 	{
// 		return 0x8000;
// 	}

// 	Tag = pTag[0];
// 	if(Tag[0] == Structure_698)
// 	{
// 		OutBuf[Lenth++] = Structure_698;
// 		OutBuf[Lenth++] = Tag[1];
// 		Num = Tag[1];
// 		Tag += 2;

// 		for (i = 0; i < Num; i++)
// 		{
// 			Lenth += AddTagBlueth1(&Tag, InBuf, &OutBuf[Lenth], OutLen - Lenth);
// 			if(Lenth >= 0x8000)
// 			{
// 				return 0x8000;
// 			}
// 		}
// 	}
// 	else if(Tag[0] == Array_698)
// 	{
// 		return 0x8000;
// 	}
// 	else 
// 	{
// 		Lenth = GetTypeLen( eTagData, Tag );
// 		if( Lenth == 0x8000 )
// 		{
// 			return 0x8000;
// 		}
		
// 		if( OutLen < Lenth )
// 		{
// 			return 0;
// 		}

// 		Lenth = GetRequestAddTag( Tag, InBuf[0], OutBuf );
// 		if( Lenth == 0x8000 )
// 		{
// 			return 0x8000;
// 		}

// 		extern TTagBuf TagBuf;
// 		InBuf[0] += TagBuf.InBufLenth;

// 		Tag += 2;
// 	}
// 	pTag[0] = Tag;

// 	return Lenth;
// }

// //--------------------------------------------------
// //功能描述:  增加蓝牙TAG
// //
// //参数: pTag[in]: TAG地址
// //		InBuf[in]: 需要加TAG的数据的地址 的地址
// //		OutBuf[in]: 输出缓冲
// //		OutLen[in]: 输出缓冲的长度
// //返回值: 加完TAG的长度
// //
// //备注: 递归调用
// //--------------------------------------------------
// static WORD AddTagBlueth(const BYTE *Tag, BYTE **InBuf, BYTE *OutBuf, WORD OutLen)
// {
// 	s_AddTagBluethCount = 0;
// 	return AddTagBlueth1(&Tag, InBuf, OutBuf, OutLen);
// }

//--------------------------------------------------
//功能描述: 对输入输出设备（class22）属性数据添加Tag
//         
//参数:
//         	*pOAD[in]：		OAD
//			Sch[in]：		OtherObj中的事件索引         
//          *InBuf[in]：	需要添加tag的数据         
//          OutBufLen[in]：	申请的缓冲长度         
//          *OutBuf[in]：	输出数据
//         
//返回值:   数据长度
//         
//备注内容: 
//--------------------------------------------------
static WORD AddTagOtherClass22Data(BYTE *pOAD, BYTE Sch, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	TTwoByteUnion OI;
	BYTE ClassAttribute,ClassIndex,i,Num,Flag,End,Start;
	WORD Result,Lenth;
	const BYTE *Tag;
//	T_ble_para pPara_inf;

	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);//bit0…bit4编码表示对象属性编号；
	ClassIndex = pOAD[3];
	Lenth = 0;
	Flag = 0;
	
	switch( ClassAttribute )
	{
		case 0x02: //设备对象列表
			switch( OI.w )
			{
				case 0xF201://RS485
				case 0xF202://IR
				case 0xF203://遥信
				case 0xF205://继电器输出
				case 0xF207://多功能端子
                case 0xF221://CAN
					if( OI.w == 0xf201 )
					{
						
                        for(i=0;i<MAX_COM_CHANNEL_NUM;i++)
                        {
                            if((SerialMap[i].SerialType==eRS485_I) /*|| (SerialMap[i].SerialType == eRS485_II)*/ )
                            {
                                Flag++;
                            }
                        }
                        
                        if( Flag == 0 )
                        {
                            return 0x8000;
                        }

                        TagPara.Array_Struct_Num = Flag;
                        #if( MAX_PHASE != 1)
                        TagPara.Lenth.w = (sizeof(PortDescript4)-1);
                        #else
                        TagPara.Lenth.w = (sizeof(PortDescript4)-1);
                        #endif
                        
						if( ClassIndex > Flag )
						{
							return 0x8000;
						}

						Tag = T_RS485;
					}
					else if( OI.w == 0xf202 )
					{
						if( ClassIndex > 1 )
						{
							return 0x8000;
						}
						
						TagPara.Lenth.w = (sizeof(PortDescript1)-1);
						
						Tag = T_IR;
					}
					else if( OI.w == 0xF203 )
					{
						if( ClassIndex > 1 )
						{
							return 0x8000;
						}
						Tag = T_SwitchIn_2;	
					}	
					else if( OI.w == 0xf205 )
					{
						if( ClassIndex > 1 )
						{
							return 0x8000;
						}
						
                        TagPara.Lenth.w = (sizeof(Relayscript)-1);

						Tag = T_RelayOut;
					}
					else if( OI.w == 0xF221 )
					{
						if( ClassIndex > 1 )
						{
							return 0x8000;
						}
						TagPara.Lenth.w = (sizeof(PortDescript5)-1);
						
						Tag = T_CAN;
					}
					else
					{
						if( ClassIndex > 1 )
						{
							return 0x8000;
						}

						Tag = T_MultiFunPort;
					}

					Tag += GetTagOffsetAddr( ClassIndex, Tag);
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
					break;	

				case 0xF209://载波、微功率无线接口
					if( ClassIndex > 1 )
					{
						return 0x8000;
					}
					TagPara.Array_Struct_Num = 1;
					TagPara.Lenth.w = ( sizeof(PortDescript2) - 1 );
					
					Tag = T_CR;
					Tag += GetTagOffsetAddr( ClassIndex, Tag);
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
					break;	
				// case 0xF20B:		//蓝牙属性2 蓝牙模块
				// 	if(ClassIndex != 0)
				// 	{
				// 		Lenth = AddTagBlueth(T_BluetoothModule, &InBuf, OutBuf, OutBufLen);
				// 	}
				// 	else
				// 	{
				// 		Num = 0x01;	//蓝牙模块的个数
				// 		OutBuf[Lenth++]= Array_698;
				// 		OutBuf[Lenth++] = Num;	//蓝牙模块的个数
				// 		for (i = 0; i < Num; i++)
				// 		{
				// 			Lenth += AddTagBlueth(T_BluetoothModule, &InBuf, &OutBuf[Lenth], OutBufLen - Lenth);
				// 			if(Lenth >= 0x8000 )
				// 			{
				// 				return 0x8000;
				// 			}
				// 		}
				// 	}
				// 	break;
				default:
					return 0x8000;
			}
			break;	

		case 3:
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			
			Tag = T_Unsigned;
			Lenth = GetTypeLen( eTagData, Tag );
			if( Lenth == 0x8000 )
			{
                return 0x8000;
			}
			
			Result = GetRequestAddTag( Tag, InBuf, OutBuf );
			if( Result == 0x8000 )
			{
				return 0x8000;
			}		
			break;
		case 4:
			// if(OI.w == 0xF20B)		//蓝牙属性4 从设备列表
			// {
			// 	if(ClassIndex != 0)
			// 	{
			// 		Lenth = AddTagBlueth(T_BluetoothDevice, &InBuf, OutBuf, OutBufLen);
			// 	}
			// 	else
			// 	{
			// 		Num = 3;	//蓝牙模块的个数
			// 		OutBuf[Lenth++]= Array_698;
			// 		OutBuf[Lenth++] = Num;	//蓝牙模块的个数
			// 		for (i = 0; i < Num; i++)
			// 		{
			// 			Lenth += AddTagBlueth(T_BluetoothDevice, &InBuf, &OutBuf[Lenth], OutBufLen - Lenth);
			// 			if(Lenth >= 0x8000 )
			// 			{
			// 				return 0x8000;
			// 			}
			// 		}
			// 	}
			// }
			// else 
			if(OI.w == 0xF203)
			{
				if( ClassIndex != 0 )
				{
					return 0x8000;
				}
				else 
				{
					Tag = T_SwitchIn_4;	
					Lenth = GetTypeLen( eTagData, Tag );
					if( Lenth == 0x8000 )
					{
                		return 0x8000;
					}
			
					Result = GetRequestAddTag( Tag, InBuf, OutBuf );
					if( Result == 0x8000 )
					{
						return 0x8000;
					}		
				}
			}
			else
			{
				return 0x8000;
			}
			break;
		case 5:
//			if(OI.w == 0xF20B)		//蓝牙属性5, 连接信息 只读
//			{
//				const BYTE T_ConnectInfo[]=
//				{
//					OAD_698,		4,	//端口号
//				};
//
//				if(ClassIndex == 0)
//				{
//					i = 0;
//					Num = 0x01;	//蓝牙模块的个数
//					OutBuf[Lenth++]= Array_698;
//					OutBuf[Lenth++] = Num;	//蓝牙模块的个数
//				}
//				else
//				{
//					i = 0;			//开始返回的连接信息索引
//					Num = i + 1;	//结束返回的连接信息索引
//				}
//				api_mt_get_ble_para_info(&pPara_inf);
//				for (i = i; i < Num; i++)
//				{
//					OutBuf[Lenth++] = Structure_698;
//					OutBuf[Lenth++] = 3;
//
//					//端口号 OAD
//					Lenth += AddTagBlueth(&T_ConnectInfo[0], &InBuf, &OutBuf[Lenth], OutBufLen - Lenth);
//					if(Lenth >= 0x8000)
//					{
//						return 0x8000;
//					}
//
//					End = 0;
//					for ( Start = 0; Start < 2; Start++)
//					{
//						if (pPara_inf.connect_info.status&(BIT0<<Start))
//						{
//							End++;
//						}
//						
//					}
//					//连接的主设备 BluetoothInfo 
//					OutBuf[Lenth++] = Array_698;
//					OutBuf[Lenth++] = End;
//					for (Start = 0; Start < End; Start++)	
//					{
//						Lenth += AddTagBlueth(&T_BluetoothModule[4], &InBuf, &OutBuf[Lenth], OutBufLen - Lenth);
//						if(Lenth >= 0x8000)
//						{
//							return 0x8000;
//						}
//					}
//
//					End = 0;
//					for ( Start = 2; Start < 5; Start++)
//					{
//						if (pPara_inf.connect_info.status&(BIT0<<Start))
//						{
//							End++;
//						}
//						
//					}
//					//连接的从设备 BluetoothInfo
//					OutBuf[Lenth++] = Array_698;
//					OutBuf[Lenth++] = End;
//					for (Start = 0; Start < End; Start++)	
//					{
//						Lenth += AddTagBlueth(&T_BluetoothModule[4], &InBuf, &OutBuf[Lenth], OutBufLen - Lenth);
//						if(Lenth >= 0x8000)
//						{
//							return 0x8000;
//						}
//					}
//				}
//			}
//			else
			{
				return 0x8000;
			}
			break;
		case 6:
			if (OI.w==0xF221)
			{
				*(OutBuf++)=Unsigned_698;
				*(OutBuf++)=*(InBuf);
				Lenth=2;
			}
			// else if(OI.w == 0xF20B)	//蓝牙属性6， 工作参数
			// {
			// 	if(ClassIndex != 0)
			// 	{
			// 		Lenth = AddTagBlueth(T_BluetoothWork, &InBuf, OutBuf, OutBufLen);
			// 	}
			// 	else
			// 	{
			// 		Num = 0x01;	//蓝牙模块的个数
			// 		OutBuf[Lenth++]= Array_698;
			// 		OutBuf[Lenth++] = Num;	//蓝牙模块的个数
			// 		for (i = 0; i < Num; i++)
			// 		{
			// 			Lenth += AddTagBlueth(T_BluetoothWork, &InBuf, &OutBuf[Lenth], OutBufLen - Lenth);
			// 			if(Lenth >= 0x8000 )
			// 			{
			// 				return 0x8000;
			// 			}
			// 		}
			// 	}
			// }
			else
			{
				return 0x8000;
			}
			break;
		case 7:
		if (OI.w==0xF221)
		{
			if(ClassIndex != 0)
			{
				if(ClassIndex > CanBusPara.bRxNum)
				{
					*(OutBuf++) = NULL_698;
					Lenth = 1;
				}
				else
				{
					*(OutBuf++) = Unsigned_698;
					*(OutBuf++) = *(InBuf + ClassIndex - 1);
					Lenth = 2;
				}
			}
			else
			{
				*(OutBuf++) = Array_698;
				*(OutBuf++) = CanBusPara.bRxNum;
				for(i = 0; i < CanBusPara.bRxNum; i++)
				{
					*(OutBuf++) = Unsigned_698;
					*(OutBuf++) = *(InBuf++);
				}
				Lenth = 2 * (CanBusPara.bRxNum + 1);
			}
		}
		// else if(OI.w == 0xF20B)		//蓝牙属性7 锁具列表
		// {
		// 	if(ClassIndex != 0)
		// 	{
		// 		Lenth = AddTagBlueth(T_BluetoothDevice, &InBuf, OutBuf, OutBufLen);
		// 	}
		// 	else
		// 	{
		// 		Num = 0x01;	//蓝牙模块的个数
		// 		OutBuf[Lenth++]= Array_698;
		// 		OutBuf[Lenth++] = Num;	//蓝牙模块的个数
		// 		for (i = 0; i < Num; i++)
		// 		{
		// 			Lenth += AddTagBlueth(T_BluetoothDevice, &InBuf, &OutBuf[Lenth], OutBufLen - Lenth);
		// 			if(Lenth >= 0x8000 )
		// 			{
		// 				return 0x8000;
		// 			}
		// 		}
		// 	}
		// }
		else
		{
			return 0x8000;
		}
		
			break;
		default:
			return 0x8000;
	}	
	
	return Lenth;
}

//--------------------------------------------------
//功能描述: 获取输入输出设备（class22）数据长度
//         
//参数:     
//          DataType[in]：eData/eTagData
//          *pOAD[in]     OAD    
//         
//返回值:   返回数据长度
//         
//备注内容: 
//--------------------------------------------------
static WORD GetRequestOtherClass22Len( BYTE DataType, BYTE *pOAD)
{
	BYTE ClassAttribute,ClassIndex,i,Flag;
	const BYTE *Tag;
	TTwoByteUnion OI;
	WORD Lenth;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);//bit0…bit4编码表示对象属性编号；
	ClassIndex = pOAD[3];
	Lenth = 0;
    Flag = 0;

	switch( ClassAttribute )
	{
		case 0x02: //设备对象列表
			switch( OI.w )
			{
				case 0xF201://RS485
				case 0xF202://IR
				//case 0xF203://开关量输入
				case 0xF205://继电器输出
				case 0xF207://多功能端子
				//case 0xF20B://蓝牙
					if( OI.w == 0xf201 )
					{
						if( ClassIndex > 2 )
						{
							return 0x8000;
						}
						
                        for(i=0;i<MAX_COM_CHANNEL_NUM;i++)
                        {
                            if((SerialMap[i].SerialType==eRS485_I) /*|| (SerialMap[i].SerialType == eRS485_II)*/ )
                            {
                                Flag++;
                            }
                        }

                        if( Flag == 0 )
                        {
                            return 0x8000;
                        }

                        TagPara.Array_Struct_Num = Flag;
                        #if( MAX_PHASE != 1)
                        TagPara.Lenth.w = (sizeof(PortDescript4)-1);
                        #else
                        TagPara.Lenth.w = (sizeof(PortDescript4)-1);
                        #endif

						Tag = T_RS485;
					}
					else if( OI.w == 0xf202 )
					{
						if( ClassIndex > 1 )
						{
							return 0x8000;
						}
						
						TagPara.Lenth.w = (sizeof(PortDescript1)-1);
						
						Tag = T_IR;
					}
					else if( OI.w == 0xf205 )
					{
						if( ClassIndex > 1 )
						{
							return 0x8000;
						}
						
                        TagPara.Lenth.w = (sizeof(Relayscript)-1);

						Tag = T_RelayOut;
					}
					else if( OI.w == 0xf221 )
					{
						if( ClassIndex > 1 )
						{
							return 0x8000;
						}
						
                        TagPara.Lenth.w = (sizeof(PortDescript5)-1);

						Tag = T_CAN;
					}
					else
					{
						if( ClassIndex > 1 )
						{
							return 0x8000;
						}

						Tag = T_MultiFunPort;
					}

					Tag +=GetTagOffsetAddr( ClassIndex, Tag);
					Lenth = GetTypeLen( DataType, Tag );
					break;	
				case 0xF209://载波、微功率无线接口
					if( ClassIndex > 1 )
					{
						return 0x8000;
					}

					// if( ClassIndex == 0 )
					// {
					// 	Lenth = (DataType == eData) ? ( 1+5+1) : ( 14 );
					// }
					// else
					// {
					// 	Lenth = (DataType == eData) ? ( 1+5+1) : ( 12 );
					// }
					TagPara.Array_Struct_Num = 1;
					TagPara.Lenth.w = (sizeof(PortDescript2)-1);
					Tag = T_CR;
					Tag +=GetTagOffsetAddr( ClassIndex, Tag);
					Lenth = GetTypeLen( DataType, Tag );
					break;
				
				default:
					return 0x8000;
			}
			break;
		default:
			return 0x8000;
	}	
	
	return Lenth;
}

//--------------------------------------------------
//功能描述: 获取显示接口类（class17）属性数据（不带Tag）
//         
//参数:
//			Dot[in]：	小数点 （未用到）    
//          *pOAD[in]：	OAD         
//          Sch[in]：   OtherObj中的事件索引      
//          *pBuf[out]：输出数据
//         
//返回值:   数据长度
//         
//备注内容: 
//--------------------------------------------------
static WORD GetOtherClass17Data( BYTE Dot, BYTE *pOAD, BYTE Sch, BYTE *pBuf )
{
	BYTE ClassAttribute,ClassIndex,i,Num;
	TTwoByteUnion OI;	
	WORD Type,Result;
	BYTE *pBufBak = pBuf;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);//bit0…bit4编码表示对象属性编号；
	ClassIndex = pOAD[3];
	
	switch( ClassAttribute )
	{
		case 0x02: //显示对象列表		
			Type = (OI.w - 0xf300);

			if( ClassIndex > FPara1->LCDPara.DispItemNum[Type] )
			{
				return 0x8000;
			}
			
			i = (ClassIndex==0) ? 0 : (ClassIndex-1);
			Num = (ClassIndex==0) ? FPara1->LCDPara.DispItemNum[Type] : 1;
			Num += i;
			
			for( ; i<Num; i++ )
			{
				Result = api_ReadLcdItem( Type,i,pBuf);
				if(Result == FALSE)
				{
					return 0x8000;
				}

				pBuf += 10;
			}
			break;

		case 0x03://每个对象显示时间
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}

			if( OI.w == 0xf300 )
			{
				memcpy( pBuf, (BYTE*)&FPara1->LCDPara.LoopDisplayTime, 2 );
				pBuf += 2;
			}
			else
			{
				memcpy( pBuf, (BYTE*)&FPara1->LCDPara.KeyDisplayTime, 2 );
				pBuf += 2;
			}		
			break;

		case 0x04://显示参数
			if( ClassIndex > 2 )
			{
				return 0x8000;
			}

			if( (ClassIndex==0) || (ClassIndex==1) )
			{
				if( OI.w == 0xf300 )
				{
					*(pBuf++) = FPara1->LCDPara.DispItemNum[0];
				}
				else
				{
					*(pBuf++) = FPara1->LCDPara.DispItemNum[1];
				}
			}

			if( (ClassIndex==0) || (ClassIndex==2) )
			{
				if( OI.w == 0xf300 )
				{
					*(pBuf++) = MAX_LCD_LOOP_ITEM;
				}
				else
				{
					*(pBuf++) =	MAX_LCD_KEY_ITEM;
				}
			}
			break;
		default:
			return 0x8000;
	}

	return  (WORD)(pBuf-pBufBak);
}

//--------------------------------------------------
//功能描述: 获取文件接口类（class18）属性数据（不带Tag）
//         
//参数:
//			Ch[in]：	通道   
//          *pOAD[in]：	OAD         
//          Sch[in]：   OtherObj中的事件索引      
//          *pBuf[out]：输出数据
//         
//返回值:   数据长度
//         
//备注内容: 
//--------------------------------------------------
static WORD GetOtherClass18Data( BYTE Ch, BYTE *pOAD, BYTE Sch, BYTE *pBuf )
{
	BYTE ClassAttribute,ClassIndex;
	TTwoByteUnion OI;	
	WORD Len = 0,wTemp;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);//bit0…bit4编码表示对象属性编号；
	ClassIndex = pOAD[3];
	
	if(OI.w != 0xF001)
	{
		return 0x8000;
	}

	switch( ClassAttribute )
	{
		case 0x04: //传输块状态字 bit_string
			if (tIapInfo.dwFlag != IAP_START)
			{
				return 0x8000;
			}
			Len = ((tIap.wAllFrameNo-1)/8)+1;
			memcpy(pBuf, IapStatus, Len);
			lib_InverseData(pBuf, Len);
			break;
		case 0x05: //下载方的标识 octet-string
	
			break;
		case 0x06: //支持传输块大小 long unsigned

			break;
		case 0x07: //执行升级时间 date time s

			break;
		default:
			return 0x8000;
	}

	return Len;
}

//--------------------------------------------------
//功能描述: 对显示接口类（class17）属性数据添加Tag
//         
//参数:
//         	*pOAD[in]：		OAD
//			Sch[in]：		OtherObj中的事件索引         
//          *InBuf[in]：	需要添加tag的数据         
//          OutBufLen[in]：	申请的缓冲长度         
//          *OutBuf[in]：	输出数据
//         
//返回值:   数据长度
//         
//备注内容: 
//--------------------------------------------------
static WORD AddTagOtherClass17Data(BYTE *pOAD, BYTE Sch, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE ClassAttribute,ClassIndex,i,Num,LenOffest;
	TTwoByteUnion OI,Len;
	WORD Result,Lenth,Type1;
	BYTE *pBufBak = OutBuf;
	const BYTE *Tag;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);//bit0…bit4编码表示对象属性编号；
	ClassIndex = pOAD[3];
	Lenth = 0;
	
	switch( ClassAttribute )
	{
		case 0x02: //显示对象列表		
			Type1 = (OI.w - 0xf300);
			
			if( ClassIndex > FPara1->LCDPara.DispItemNum[Type1] )
			{
				return 0x8000;
			}
	
			if( ClassIndex == 0 )
			{
				
				
				*(OutBuf++) = Array_698;
				Len.w = FPara1->LCDPara.DispItemNum[Type1];
				LenOffest = Deal_698DataLenth( OutBuf, Len.b, eUNION_TYPE ); 
				OutBuf += LenOffest;
			}
			else
			{
				if( OutBufLen < 15 )
				{
					return 0;
				}
			}
			
			Num = (ClassIndex==0) ? FPara1->LCDPara.DispItemNum[Type1] : 1;
			
			for( i=0; i<Num; i++ )
			{
				if((OutBufLen-(WORD)(OutBuf-pBufBak))<15)
				{
					return 0;
				}
				*(OutBuf++) = Structure_698;
				*(OutBuf++) = 2;
				if( InBuf[10*i+8] == 1 )
				{
					*(OutBuf++) = CSD_698;
					*(OutBuf++) = 0;
					memcpy( OutBuf, InBuf+(10*i), 4 );
					OutBuf += 4;
				}
				else
				{
					*(OutBuf++) = CSD_698;
					*(OutBuf++) = 1;
					memcpy( OutBuf, InBuf+(10*i), 4 );
					OutBuf += 4;
					*(OutBuf++) = 1;
					memcpy( OutBuf, InBuf+(10*i+4), 4 );
					OutBuf += 4;
				}

				*(OutBuf++) = Unsigned_698;
				*(OutBuf++) = InBuf[10*i+9];
			}
			
			Lenth = (WORD)(OutBuf-pBufBak);			
			break;

		case 0x03://每个对象显示时间
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			
			Lenth = GetTypeLen( eTagData, T_UNLong );
		    if( Lenth == 0x8000 )
			{
                return 0x8000;
			}
			
			if( OutBufLen < Lenth )
			{
				return 0;
			}

			Result = GetRequestAddTag( T_UNLong, InBuf, OutBuf );
			if( Result == 0x8000 )
			{
				return 0x8000;
			}		
			break;

		case 0x04://显示参数
			if( ClassIndex > 2 )
			{
				return 0x8000;
			}
			
			Tag = T_Display;
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
			break;
		
		default:
			return 0x8000;
	}	
	
	return Lenth;
}

//--------------------------------------------------
//功能描述: 对显示接口类（class17）属性数据添加Tag
//         
//参数:
//         	*pOAD[in]：		OAD
//			Sch[in]：		OtherObj中的事件索引         
//          *InBuf[in]：	需要添加tag的数据         
//          OutBufLen[in]：	申请的缓冲长度         
//          *OutBuf[in]：	输出数据
//         
//返回值:   数据长度
//         
//备注内容: 
//--------------------------------------------------
static WORD AddTagOtherClass18Data(BYTE *pOAD, BYTE Sch, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE ClassAttribute, ClassIndex;
	TTwoByteUnion OI;
	WORD Result, Lenth;
	const BYTE *Tag;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);//bit0…bit4编码表示对象属性编号；
	ClassIndex = pOAD[3];
	Lenth = 0;
	
	switch( ClassAttribute )
	{
		case 0x04: //传输块状态字 bit_string
			Tag = T_TransfeStatus;
//			TagPara.Lenth.w = (((tIap.wAllFrameNo-1)/8)+1)*8;
            TagPara.Lenth.w = tIap.wAllFrameNo;
			break;
		case 0x05: //下载方的标识 octet-string
			TagPara.Lenth.w = 0x10;	//octet-string的长度

			Tag = T_DownloadID;
			break;
		case 0x06: //支持传输块大小 long unsigned
			Tag = T_BlockSize;
			break;
		case 0x07: //执行升级时间 date time s	
			Tag = T_UpdatTime;
			break;
		default:
			return 0x8000;
	}	
	
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

//--------------------------------------------------
//功能描述: 获取显示接口类（class17）数据长度
//         
//参数:     
//          DataType[in]：eData/eTagData
//          *pOAD[in]     OAD    
//         
//返回值:   返回数据长度
//         
//备注内容: 
//--------------------------------------------------
static WORD GetRequestOtherClass17Len( BYTE DataType, BYTE *pOAD)
{
	BYTE ClassAttribute,ClassIndex;
	const BYTE *Tag;
	TTwoByteUnion OI;
	WORD Lenth,Type1;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);//bit0…bit4编码表示对象属性编号；
	ClassIndex = pOAD[3];
	Lenth = 0;

	switch( ClassAttribute )
	{
		case 0x02: //显示对象列表		
			Type1 = (OI.w - 0xf300);
			
			if( ClassIndex > FPara1->LCDPara.DispItemNum[Type1] )
			{
				return 0x8000;
			}
	
			if( ClassIndex == 0 )
			{
				Lenth = ( DataType == eData) ? (10*FPara1->LCDPara.DispItemNum[Type1]) : (15*FPara1->LCDPara.DispItemNum[Type1]+2);
			}
			else
			{
				Lenth = ( DataType == eData) ? 10 : 15;
			}				
			break;

		case 0x03://每个对象显示时间
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}	
			
			Lenth = GetTypeLen( DataType, T_UNLong );	
			break;

		case 0x04://显示参数
			if( ClassIndex > 2 )
			{
				return 0x8000;
			}
			
			Tag = T_Display;
			Tag += GetTagOffsetAddr( ClassIndex, Tag);
			Lenth = GetTypeLen( DataType, Tag );
			break;
		
		default:
			return 0x8000;
	}	
	
	return Lenth;
}

//--------------------------------------------------
//功能描述: 获取其他接口类属性数据
//         
//参数:
//         	DataType[in]：	eData/eTagData/eAddTag
//         	Dot[in]:		小数点        
//         	*pOAD[in]：		OAD         
//         	*InBuf[in]：	需要添加tag的数据         
//         	OutBufLen[in]：	申请的缓冲长度         
//         	*OutBuf[out]：	输出数据
//         
//返回值:   数据长度
//         
//备注内容: OtherObj中支持的数据
//--------------------------------------------------
WORD GetOtherData( BYTE Ch, BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{	
	BYTE Sch,ClassAttribute;
	BYTE Buf[MAX_OTHER_BUF_LENTH];
	TTwoByteUnion OI;
	WORD Lenth;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1F);//bit0…bit4编码表示对象属性编号；
	
	Sch = SearchOtherOAD( OI.w );
	if( Sch == 0x80 )
	{
		return 0x8000;
	}

	if( ClassAttribute > OtherObj[Sch].MAXClassAttribute )
	{
		return 0x8000;
	}
	
	if( (DataType==eData) || (DataType==eTagData) )
	{
		if( OtherObj[Sch].Class == eclass8 )
		{
			Lenth = GetOtherClass8Data( Dot, pOAD, Sch, Buf );
		}
		#if(SEL_ESAM_TYPE != 0)
		else if( OtherObj[Sch].Class == eclass21 )
		{
			Lenth = GetOtherClass21Data( Ch, Dot, pOAD, Sch, Buf );
		}
		#endif
		else if( OtherObj[Sch].Class == eclass22 )
		{
			Lenth = GetOtherClass22Data( Dot, pOAD, Sch, Buf );
		}
		// else if( OtherObj[Sch].Class == eclass17 )
		// {
		// 	Lenth = GetOtherClass17Data( Dot, pOAD, Sch, Buf );
		// }
		else if( OtherObj[Sch].Class == eclass18 )
		{
			Lenth = GetOtherClass18Data( Ch, pOAD, Sch, Buf );
		}
		else
		{
            return 0x8000;
		}
	
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
			if( OtherObj[Sch].Class == eclass8 )
			{
				Lenth = AddTagOtherClass8Data(pOAD, Sch, Buf, OutBufLen, OutBuf);
			}
			else if( OtherObj[Sch].Class == eclass21 )
			{
				Lenth = AddTagOtherClass21Data(pOAD, Sch, Buf, OutBufLen, OutBuf);
			}
			else if( OtherObj[Sch].Class == eclass22 )
			{
				Lenth = AddTagOtherClass22Data(pOAD, Sch, Buf, OutBufLen, OutBuf);
			}
			// else if( OtherObj[Sch].Class == eclass17 )
			// {
			// 	Lenth = AddTagOtherClass17Data(pOAD, Sch, Buf, OutBufLen, OutBuf);
			// }    	
			else if( OtherObj[Sch].Class == eclass18 )
			{
				Lenth = AddTagOtherClass18Data(pOAD, Sch, Buf, OutBufLen, OutBuf);
			}    
			else
    	    {
                return 0x8000;
    	    }
		}
	}
	else if( DataType == eAddTag )//冻结类不支持单独加TAG
	{
		if( InBuf == NULL )
		{
			return 0x8000;
		}
		
		if( OtherObj[Sch].Class == eclass8 )
		{
			Lenth = AddTagOtherClass8Data(pOAD, Sch, InBuf, OutBufLen, OutBuf);
		}
		else if( OtherObj[Sch].Class == eclass21 )
		{
			Lenth = AddTagOtherClass21Data(pOAD, Sch, InBuf, OutBufLen, OutBuf);
		}
		else if( OtherObj[Sch].Class == eclass22 )
		{
			Lenth = AddTagOtherClass22Data(pOAD, Sch, InBuf, OutBufLen, OutBuf);
		}
		// else if( OtherObj[Sch].Class == eclass17 )
		// {
		// 	Lenth = AddTagOtherClass17Data(pOAD, Sch, InBuf, OutBufLen, OutBuf);
		// }
		else if( OtherObj[Sch].Class == eclass18 )
		{
			Lenth = AddTagOtherClass18Data(pOAD, Sch, Buf, OutBufLen, OutBuf);
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

	return Lenth; 
}

//--------------------------------------------------
//功能描述: 获取其他接口类的数据长度
//         
//参数:     
//          DataType[in]：eData/eTagData
//          *pOAD[in]     OAD    
//         
//返回值:   返回数据长度
//         
//备注内容: 
//--------------------------------------------------
WORD GetOtherDataLen( BYTE DataType, BYTE *pOAD)
{
	TTwoByteUnion OI,Len;
	BYTE Sch;
	
	Len.w = 0;
	lib_ExchangeData(OI.b,pOAD,2);
	Sch = SearchOtherOAD( OI.w );
	if( Sch == 0x80 )
	{
		return 0x8000;
	}
	
	if( OtherObj[Sch].Class == eclass8 )
	{
		Len.w = GetRequestOtherClass8Len( DataType, pOAD );
	}
	else if( OtherObj[Sch].Class == eclass21 )
	{
		Len.w = GetRequestOtherClass21Len( DataType, pOAD );
	}
	else if( OtherObj[Sch].Class == eclass22 )
	{
		Len.w = GetRequestOtherClass22Len( DataType, pOAD );
	}
	// else if( OtherObj[Sch].Class == eclass17 )
	// {
	// 	Len.w = GetRequestOtherClass17Len( DataType, pOAD );
	// }
	else
	{
		return 0x8000;
	}

	return Len.w;
}

#endif//#if ( SEL_DLT698_2016_FUNC == YES)

