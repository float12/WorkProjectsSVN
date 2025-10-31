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
#define MAX_OTHER_BUF_LENTH         (MAX_LCD_KEY_ITEM*10+30)//液晶按键显示为最大BUF长度

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
const char PortDescript2[]	= "CR";
const char PortDescript3[] 	= "RS4852";
const char PortDescript4[] 	= "RS485";
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

//增加数据类型请查看源数据和加TAG是否支持
static const TOtherObj OtherObj[] =
{
	{  0x8000, 	eclass8,	5	},	//遥控
	{  0x8001, 	eclass8,	2	},	//保电
	{  0xF100, 	eclass21,	21	},	//ESAM
	{  0xF101, 	eclass8,	3	},	//安全模式参数
	{  0xF201, 	eclass22,	2	},	//RS485
	{  0xF202, 	eclass22,	2	},	//红外
	{  0xF205, 	eclass22,	2	},	//继电器输出
	{  0xF207, 	eclass22,	2	},	//多功能端子
	{  0xF209, 	eclass22,	2	},	//载波/微功率无线接口
	{  0xF300, 	eclass17,	4	},	//自动轮显
	{  0xF301, 	eclass17,	4	},	//按键轮显
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
						if((SerialMap[i].SerialType==eRS485_I) || (SerialMap[i].SerialType == eRS485_II) )
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
					TagPara.Lenth.w = (sizeof(PortDescript0)-1);
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
							memcpy(pBuf,PortDescript0,(sizeof(PortDescript0)-1));//端口描述符
							pBuf += (sizeof(PortDescript0)-1);
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

					TagPara.Lenth.w = 1;
					*(pBuf++) = 0x00;

					TempCommPara = FPara2->CommPara.ComModule;//端口参数
					*(pBuf++) = (TempCommPara & 0x0F);//波特率
					*(pBuf++) = ((TempCommPara>>5) & 0x03);//校验位
					*(pBuf++) = 8;//数据位
					*(pBuf++) = (((TempCommPara>>4) & 0x01)+1);//停止位
					*(pBuf++) = 0;//流控

					*(pBuf++) = NULL;//版本信息为空
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
					if((SerialMap[i].SerialType==eRS485_I) || (SerialMap[i].SerialType == eRS485_II) )
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
		default:
			return 0x8000;
	}

	return  (WORD)(pBuf-pBufBak);
}

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
	BYTE ClassAttribute,ClassIndex,i,Flag;
	WORD Result,Lenth;
	const BYTE *Tag;
	
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
				case 0xF205://继电器输出
				case 0xF207://多功能端子
					if( OI.w == 0xf201 )
					{
						
                        for(i=0;i<MAX_COM_CHANNEL_NUM;i++)
                        {
                            if((SerialMap[i].SerialType==eRS485_I) || (SerialMap[i].SerialType == eRS485_II) )
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
                        TagPara.Lenth.w = (sizeof(PortDescript0)-1);
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
					else if( OI.w == 0xf205 )
					{
						if( ClassIndex > 1 )
						{
							return 0x8000;
						}
						
                        TagPara.Lenth.w = (sizeof(Relayscript)-1);

						Tag = T_RelayOut;
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
					
					TagPara.Lenth.w = 1;
					
					if( ClassIndex == 0 )
					{
						if( OutBufLen < (13+TagPara.Lenth.w))
						{
							return 0;
						}
						Lenth = (13+TagPara.Lenth.w);
						*(OutBuf++) = Array_698;
						*(OutBuf++) = 01;
					}
					else
					{
						if( OutBufLen < (11+TagPara.Lenth.w))
						{
							return 0;
						}

						Lenth = (11+TagPara.Lenth.w);
					}

					*(OutBuf++) = Structure_698;
					*(OutBuf++) = 3;
					*(OutBuf++) = Visible_string_698;
					*(OutBuf++) = TagPara.Lenth.w;
					memcpy( OutBuf,InBuf,TagPara.Lenth.w);
					OutBuf += TagPara.Lenth.w; 
					InBuf += TagPara.Lenth.w;
					*(OutBuf++) = COMDCB_698;
					memcpy( OutBuf,InBuf,5);
					OutBuf += 5;
					*(OutBuf++) = 0;
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
				case 0xF205://继电器输出
				case 0xF207://多功能端子
					if( OI.w == 0xf201 )
					{
						if( ClassIndex > 2 )
						{
							return 0x8000;
						}
						
                        for(i=0;i<MAX_COM_CHANNEL_NUM;i++)
                        {
                            if((SerialMap[i].SerialType==eRS485_I) || (SerialMap[i].SerialType == eRS485_II) )
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
                        TagPara.Lenth.w = (sizeof(PortDescript0)-1);
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

					if( ClassIndex == 0 )
					{
						Lenth = (DataType == eData) ? ( 1+5+1) : ( 14 );
					}
					else
					{
						Lenth = (DataType == eData) ? ( 1+5+1) : ( 12 );
					}
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
		else if( OtherObj[Sch].Class == eclass21 )
		{
			Lenth = GetOtherClass21Data( Ch, Dot, pOAD, Sch, Buf );
		}
		else if( OtherObj[Sch].Class == eclass22 )
		{
			Lenth = GetOtherClass22Data( Dot, pOAD, Sch, Buf );
		}
		else if( OtherObj[Sch].Class == eclass17 )
		{
			Lenth = GetOtherClass17Data( Dot, pOAD, Sch, Buf );
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
			else if( OtherObj[Sch].Class == eclass17 )
			{
				Lenth = AddTagOtherClass17Data(pOAD, Sch, Buf, OutBufLen, OutBuf);
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
		else if( OtherObj[Sch].Class == eclass17 )
		{
			Lenth = AddTagOtherClass17Data(pOAD, Sch, InBuf, OutBufLen, OutBuf);
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
	else if( OtherObj[Sch].Class == eclass17 )
	{
		Len.w = GetRequestOtherClass17Len( DataType, pOAD );
	}
	else
	{
		return 0x8000;
	}

	return Len.w;
}

#endif//#if ( SEL_DLT698_2016_FUNC == YES)

