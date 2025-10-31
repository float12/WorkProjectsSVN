//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部
//创建人	魏灵坤
//创建日期	2016.9.2
//描述		国标DL/T645-2007规约源文件
//修改记录
//----------------------------------------------------------------------
#include "AllHead.h"
#include "Dlt698_45.h"

#if( SEL_DLT645_2007 == YES )
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------

//-----------------------------------------------

//-----------------------------------------------
//				本文件使用的变量，常量
TDLT645APPFollow 			DLT645APPFollow[MAX_COM_CHANNEL_NUM];
T_DLT645 					gPr645[MAX_COM_CHANNEL_NUM];
TDLT645Sendbuf 				DLT645Sendbuf[MAX_COM_CHANNEL_NUM];

__no_init BYTE 				g_645PassWordErrCounter[2];//密码错误次数03 04 上电不初始化

//认证不通过，只能读出表号、通信地址、备案号、当前日期、当前时间、当前日期时间、当前电能、当前剩余金额、红外认证、查询命令，
const DWORD g_dwHongwaiID[] = {0x04000402,0x04000401,0x04800004,0x04000101,0x04000102,0x0400010c,0x00900200,0x078003FF,0x070003FF};
const DWORD FirstDataID[] = {0x04000108,0x04000109,0x04000306,0x04000307,0x04000402,0x0400040E,0x04001001,0x04001002,0x040502ff,0x040605FF,0x04060AFF,0x04060BFF};//确认一下songchen

//-----------------------------------------------
//				内部函数声明
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
void PowerUpDlt645(void)
{
    api_ClrSysStatus(eSYS_STATUS_ID_645AUTH);
}

//--------------------------------------------------
//功能描述:断开身份认证
//         
//参数  :   无
//
//返回值:    无   
//         
//备注内容:  无
//--------------------------------------------------
void api_Release645_Auth( void )
{
    api_ClrSysStatus(eSYS_STATUS_ID_645AUTH);
}

//-----------------------------------------------
//功能描述  :   初始化645参数
//
//参数  : 	无
//
//返回值:      无
//
//备注内容  :   无
//-----------------------------------------------
void FactoryInitDLT645(void)
{
	memset( g_645PassWordErrCounter, 0x00, sizeof(g_645PassWordErrCounter) );
}

WORD Judge645AddrCanDo( eSERIAL_TYPE PortType )
{
	if( gPr645[PortType].eProtocolType != ePROTOCOL_DLT645_07 )//不是07规约默认返回成功
	{
		return DLT645_OK_00;
	}
	
	if( gPr645[PortType].eAddrType == eTYPE_ADDR_TYPE_99 )//全99地址
	{
		switch( gPr645[PortType].byReceiveControl )//支持的命令
		{
			case eCONTROL_RECE_08://广播校时
			case eCONTROL_RECE_16://冻结
			case eCONTROL_RECE_1D://设置多功能端子				
			case 0x1f://东软载波
			case eCONTROL_RECE_11://读命令(液晶查看)
				return DLT645_OK_00;
			default:
			
			return DLT645_ERR_DATA_01;
		}
	}

	switch( gPr645[PortType].byReceiveControl )//掉电和带有通配地址不支持的命令
	{
		case eCONTROL_RECE_08://广播校时
		case eCONTROL_RECE_16://冻结
		case eCONTROL_RECE_14://写操作
		case eCONTROL_RECE_17://更改波特率
		case eCONTROL_RECE_19://需量清零
		case eCONTROL_RECE_1A://电能量清零
		case eCONTROL_RECE_1B://事件清零
		case eCONTROL_RECE_1C://控制命令
			if( (gPr645[PortType].eAddrType == eTYPE_ADDR_TYPE_ALLAA)//全AA地址
			  ||(gPr645[PortType].eAddrType == eTYPE_ADDR_TYPE_AA) )//通配地址
			{
				return DLT645_ERR_DATA_01;
			}
			else if( api_GetSysStatus( eSYS_STATUS_POWER_ON ) == FALSE )
			{
				return DLT645_ERR_DATA_01;
			}
		break;

		default:
			return DLT645_OK_00;	
	}

	if( gPr645[PortType].byReceiveControl == eCONTROL_RECE_13 )//读地址仅支持全AA地址
	{
		if( gPr645[PortType].eAddrType == eTYPE_ADDR_TYPE_ALLAA )//全AA地址
		{
			return DLT645_ERR_DATA_01;
		}
	}
	
	return DLT645_OK_00;
}

//---------------------------------------------------------------
//函数功能: 判断是否是一类数据
//
//参数: 	dwID[in]  数据标识
//                   
//返回值:   TRUE/FALSE
//
//备注:   
//---------------------------------------------------------------
BYTE api_JudgeFirstData( DWORD dwID )
{
	BYTE i;
	for( i=0; i<(sizeof(FirstDataID)/sizeof(DWORD)); i++ )
	{
		if( dwID == FirstDataID[i] )
		{
			return TRUE;
		}
	}
	return FALSE;	

}

//--------------------------------------------------
//功能描述:  645命令权限判断
//
//参数:      eSERIAL_TYPE PortType[in]，ControlByte[in]
//
//返回值:
//			DLT645_OK_00 - 正确返回
// 			DLT645_ERR_DATA_01 ~ DLT645_ERR_PRG_KEY_80  -  错误代码
// 			
//
//备注内容:  无
//--------------------------------------------------

WORD api_JudgeAuthority(eSERIAL_TYPE PortType)
{
	BYTE i;
	BYTE ControlByte;
	BYTE *ProBuf;
	ProBuf = (BYTE *)gPr645[PortType].pMessageAddr;
	ControlByte = gPr645[PortType].byReceiveControl;

	if( ControlByte == eCONTROL_RECE_03 )
	{
		if( (PortType == eIR)&&(api_GetSysStatus( eSYS_IR_ALLOW_PRG ) == FALSE) )
		{
            for( i=0; i<(sizeof(g_dwHongwaiID)/sizeof(DWORD)); i++ )
			{
				if( gPr645[PortType].dwDataID.d == g_dwHongwaiID[i] )
				{
					return DLT645_OK_00;
				}
			}
			return DLT645_ERR_DATA_01;//就是01没错
		}	
	}
	else if( ControlByte == eCONTROL_RECE_08 )//广播校时需要判断红外权限
	{
		if( (PortType == eIR) && (api_GetSysStatus( eSYS_IR_ALLOW_PRG ) == FALSE) )
		{
			if( gPr645[PortType].eAddrType == eTYPE_ADDR_TYPE_99 )
			{
				return DLT645_NO_RESPONSE;
			}
			else
			{
				return DLT645_ERR_PASSWORD_04;
			}
		}
	}
	else if( ControlByte == eCONTROL_RECE_11 )
	{
	    if( gPr645[PortType].eProtocolType == ePROTOCOL_DLT645_FACTORY )
	    {
            return DLT645_OK_00;
	    }
		else if( (PortType == eIR)&&(api_GetSysStatus( eSYS_IR_ALLOW_PRG ) == FALSE) )
		{
            for( i=0; i<(sizeof(g_dwHongwaiID)/sizeof(DWORD)); i++ )
			{
				if( gPr645[PortType].dwDataID.d == g_dwHongwaiID[i] )
				{
					return DLT645_OK_00;
				}
			}
	
			if(( gPr645[PortType].dwDataID.b[0] == 0x00 )&&( gPr645[PortType].dwDataID.b[3] == 0x00 ))//读当前电能
			{
				return DLT645_OK_00;
			}
			
			return DLT645_ERR_PASSWORD_04;
		}	
	}
	else if( ControlByte == eCONTROL_RECE_14 )
	{
		if( !((gPr645[PortType].dwDataID.b[3])&0x04 ) )
		{
			//用户对不支持写操作的数据项(0x04XXXXXX之外的数据)编程，应答"密码错/未授权"
			return DLT645_ERR_PASSWORD_04;
		}

        if(gPr645[PortType].dwDataID.d == 0x04001503)//复位主动上报状态字不判断密级、密码
        {
            return DLT645_OK_00;
        }
        
		if( gPr645[PortType].bLevel == MINGWEN_H_PASSWORD_LEVEL )//02 级
		{
			if( api_GetSysStatus( eSYS_STATUS_INSIDE_FACTORY ) == FALSE )//非厂内不能设置
			{
			    if( gPr645[PortType].eProtocolType == ePROTOCOL_DLT645_FACTORY )//厂内命令返回成功
				{
					return DLT645_OK_00;
				}
				else
				{
                    return DLT645_ERR_PASSWORD_04;
				}
			}
		}
		else if( gPr645[PortType].bLevel == MINGWEN_L_PASSWORD_LEVEL )//04 级
		{
			return DLT645_ERR_PASSWORD_04;//不可用来设置
		}
		else if( (gPr645[PortType].bLevel == CLASS_2_PASSWORD_LEVEL )||( gPr645[PortType].bLevel == CLASS_1_PASSWORD_LEVEL ) )//98 99 级
		{
			//身份认证在外面已经判过，不再重复
			//MAC挂起
			if( api_GetRunHardFlag(eRUN_HARD_MAC_ERR_FLAG) == TRUE )
			{
				return DLT645_ERR_PASSWORD_04;
			}
				
			if( gPr645[PortType].bLevel == CLASS_2_PASSWORD_LEVEL)//98级
			{
				if(gPr645[PortType].dwDataID.d == 0x04001503)
				{
					return DLT645_OK_00;
				}
				if( api_JudgeFirstData(gPr645[PortType].dwDataID.d) == TRUE )//如果是一类数据，采用99H级密码设置返回密码错
				{
					return DLT645_ERR_PASSWORD_04;
				}	
			}
			else//99级
			{
				if(gPr645[PortType].dwDataID.d == 0x04001503)
				{
					return DLT645_ERR_PASSWORD_04;
				}
			}
		}
	}
	else if( ControlByte == eCONTROL_RECE_1C )
	{
		//密码为98级
		if(gPr645[PortType].bLevel == CLASS_2_PASSWORD_LEVEL)
		{
			//MAC挂起
			if( api_GetRunHardFlag(eRUN_HARD_MAC_ERR_FLAG) == TRUE )
			{
				return R_ERR_MAC;
			}
		}
		else if( (gPr645[PortType].bLevel==MINGWEN_H_PASSWORD_LEVEL) || (gPr645[PortType].bLevel==MINGWEN_L_PASSWORD_LEVEL) )
		{	
			memmove( ProBuf+22, ProBuf+18, 8 );	
			lib_InverseData( ProBuf+24, 6 );
			if( (ProBuf[22]==0x1a) ||(ProBuf[22]==0x1b) || (ProBuf[22]==0x1c) || (api_GetSysStatus(eSYS_STATUS_INSIDE_FACTORY)==TRUE) )
			{
				//支持02,04级密码明文跳闸、合闸操作,不经过红外认证也可以合闸
			}
			else
			{
				return R_ERR_PASSWORD;
			}
		}
		else
		{
			return R_ERR_PASSWORD;
		}
	}

	return DLT645_OK_00;
}

//---------------------------------------------------------------
//函数功能: 判断是否有后续帧
//
//参数: 	eSERIAL_TYPE PortType
//
//返回值:
//
//备注:
//---------------------------------------------------------------
static WORD JudgeFollowFlag( eSERIAL_TYPE PortType )
{
	if( gPr645[PortType].byReceiveControl == eCONTROL_RECE_12 )//如果是读取后续帧
	{
		if( DLT645APPFollow[PortType].FollowFlag != eNO_FOLLOW )//如果有后续帧分帧且正在分帧返回正确
		{
			return DLT645_OK_00;
		}
		else
		{
			return DLT645_ERR_ID_02;//返回非法数据
		}
	}
	else
	{
		return DLT645_OK_00;
	}
}
//---------------------------------------------------------------
//函数功能: 判断解锁密码
//
//参数: 	eSERIAL_TYPE PortType
//                   
//返回值:  TRUE/FALSE
//
//备注:   
//---------------------------------------------------------------
static BOOL CheckUnlockPassword( eSERIAL_TYPE PortType )
{	
	BYTE i;
	TMuchPassword Password;

	//解锁密码仅在锁定状态下生效
	if(	(api_GetRunHardFlag(eRUN_HARD_H_PASSWORD_ERR_STATUS) == FALSE) &&
		(api_GetRunHardFlag(eRUN_HARD_L_PASSWORD_ERR_STATUS) == FALSE) &&
		(api_GetRunHardFlag(eRUN_HARD_PASSWORD_ERR_STATUS) == FALSE ))
	{
		return FALSE;	
	}

	for( i=0; i<3; i++ )
	{
		if( gPr645[PortType].bPassword[i] != 0xAB )
		{
			break;
		}
	}
	
	if( i != 3 ) //密码判断不正确
	{
		return FALSE;
	}
	else
	{
		g_645PassWordErrCounter[0] = 0;	//02级密码错误计数清0
		g_645PassWordErrCounter[1] = 0;	//04级密码错误计数清0
		api_ClrRunHardFlag(eRUN_HARD_PASSWORD_ERR_STATUS);//清零698密码错误标志
		api_ClrRunHardFlag(eRUN_HARD_H_PASSWORD_ERR_STATUS);//清零02级密码错误标志
		api_ClrRunHardFlag(eRUN_HARD_L_PASSWORD_ERR_STATUS);//清零04级密码错误标志
		return TRUE;
	}
}

//---------------------------------------------------------------
//函数功能: 判断密码!!!!!!!!重做
//
//参数: 	eSERIAL_TYPE PortType
//                   
//返回值:  TRUE/FALSE
//
//备注:   
//---------------------------------------------------------------
static BOOL CheckPassword( eSERIAL_TYPE PortType )
{
	BYTE i;
	TMuchPassword Password;

	if(gPr645[PortType].bLevel == SYSTEM_UNLOCK_PASSWORD_LEVEL)
	{
		//密码解锁已在下面函数中处理完成，默认返回FALSE
		CheckUnlockPassword(PortType);
		return FALSE;
	}
	
	//从EEPROM读取密码数据 如果EEPROM数据错误 不判密码 防止EEPROM错误或者未初始化无法打开厂内模式
	if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR(ParaSafeRom.MuchPassword), sizeof(TMuchPassword),(BYTE *)Password.Password645) != TRUE )
	{
		return TRUE;
	}

	//判断是否为02级密码厂内规约
	if( (gPr645[PortType].eProtocolType == ePROTOCOL_DLT645_FACTORY) && (gPr645[PortType].bLevel == MINGWEN_H_PASSWORD_LEVEL) )
	{
		//判断是否为打开或关闭厂内模式标识
		if( (gPr645[PortType].pDataAddr[0] == 0x01)&&
			(gPr645[PortType].pDataAddr[1] == 0x02)&&
			(gPr645[PortType].pDataAddr[2] == 0xDF)&&
			(gPr645[PortType].pDataAddr[3] == 0xDB)  )
		{
			return TRUE;
		}
	}

	for( i=0; i<3; i++ )
	{
		if( gPr645[PortType].bPassword[i] != Password.Password645[gPr645[PortType].bLevel][i] )
		{
			break;
		}
	}
	
	if( i != 3 ) //密码判断不正确
	{
		//密码验证错误 累加错误次数
		if( gPr645[PortType].bLevel == MINGWEN_H_PASSWORD_LEVEL )
		{
			if( (++g_645PassWordErrCounter[0]) >= MAX_PASSWORD_ERR_TIMES )
			{			
				api_SetRunHardFlag(eRUN_HARD_H_PASSWORD_ERR_STATUS);
			    api_WriteFreeEvent( EVENT_PROGRAMKEY_LOCK, 0 );
			}
		}
		else if( gPr645[PortType].bLevel == MINGWEN_L_PASSWORD_LEVEL )
		{
			if( (++g_645PassWordErrCounter[1]) >= MAX_PASSWORD_ERR_TIMES )
			{
				api_SetRunHardFlag(eRUN_HARD_L_PASSWORD_ERR_STATUS);
			}
		}
		return FALSE;
	}
	//密码验证正确时，错误计数清零
	else if( i == 3 )
	{
		if( gPr645[PortType].bLevel == MINGWEN_H_PASSWORD_LEVEL )
		{
			g_645PassWordErrCounter[0] = 0;
		}
		else if( gPr645[PortType].bLevel == MINGWEN_L_PASSWORD_LEVEL )
		{
			g_645PassWordErrCounter[1] = 0;
		}
	}

	//若当前为MINGWEN_H_PASSWORD_LEVEL闭锁状态，即使MINGWEN_H_PASSWORD_LEVEL密码验证正确，也要返回FALSE
	if( (api_GetRunHardFlag(eRUN_HARD_H_PASSWORD_ERR_STATUS) == TRUE) && (gPr645[PortType].bLevel == MINGWEN_H_PASSWORD_LEVEL) )
	{
		return FALSE;
	}
	//若当前为MINGWEN_L_PASSWORD_LEVEL闭锁状态，即使MINGWEN_L_PASSWORD_LEVEL密码验证正确，也要返回FALSE
	else if( (api_GetRunHardFlag(eRUN_HARD_L_PASSWORD_ERR_STATUS) == TRUE) && (gPr645[PortType].bLevel == MINGWEN_L_PASSWORD_LEVEL) )
	{
		return FALSE;
	}
	return TRUE;
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
void InitDLT645Flag( eSERIAL_TYPE PortType )
{
    if( DLT645APPFollow[PortType].FollowFlag != eREPORT_FOLLOW )
    {
        DLT645APPFollow[PortType].FollowFlag = eNO_FOLLOW;
    }
}

//--------------------------------------------------
//功能描述:  判断密码函数
//
//参数  : 无
//
//返回值:    static
//
//备注内容:  无
//--------------------------------------------------
static WORD JudgePasswordLevel( eSERIAL_TYPE PortType )
{	
	//三类参数不判断密码等级和内容 排查是否已经添加@@@@@
	//G表信号强度 复位主动上报状态字 
	//模块口扩展命令 魏灵坤提供文档@@@@

	//设置、最大需量清零、电表清零、事件清零、费控操作(跳合闸、报警、保电)
	if( (gPr645[PortType].byReceiveControl != eCONTROL_RECE_14)//只有这5种命令有密码
	  &&(gPr645[PortType].byReceiveControl != eCONTROL_RECE_19)
	  &&(gPr645[PortType].byReceiveControl != eCONTROL_RECE_1A)
	  &&(gPr645[PortType].byReceiveControl != eCONTROL_RECE_1B)
	  &&(gPr645[PortType].byReceiveControl != eCONTROL_RECE_1C) )	
	{
		return DLT645_OK_00;
	}
	
	//复位主动上报状态字 不判密码不判等级直接返回正确 写无线信号强度
	if( gPr645[PortType].dwDataID.d == 0x04001503 )
	{
		return DLT645_OK_00;
	}

	if( (gPr645[PortType].bLevel != CLASS_1_PASSWORD_LEVEL) //只支持这4种密码
	  &&(gPr645[PortType].bLevel != CLASS_2_PASSWORD_LEVEL)
	  &&(gPr645[PortType].bLevel != MINGWEN_H_PASSWORD_LEVEL) //密码等级用宏定义表示 @@@@
	  &&(gPr645[PortType].bLevel != MINGWEN_L_PASSWORD_LEVEL)
	  &&(gPr645[PortType].bLevel != SYSTEM_UNLOCK_PASSWORD_LEVEL) )//新增解锁密码	
	{
		return DLT645_ERR_PASSWORD_04;
	}

	if( api_GetSysStatus(eSYS_STATUS_INSIDE_FACTORY) == TRUE )//厂内模式下 默认权限正确
	{
        return DLT645_OK_00;
	}
	else
	{
        if( (gPr645[PortType].bLevel == CLASS_1_PASSWORD_LEVEL)//加密密级不进行密码的判断,只判断是否身份认证通过
        ||(gPr645[PortType].bLevel == CLASS_2_PASSWORD_LEVEL) )
        {
            if( api_GetSysStatus( eSYS_STATUS_ID_645AUTH ) == TRUE )
            {
                return DLT645_OK_00;
            }
            else
            {
               return DLT645_ERR_PASSWORD_04;
            }
        }
        else
        {
            if( CheckPassword( PortType ) == TRUE )//判断密码是否正确
            {
                return DLT645_OK_00;
            }
        }
	}
	
	return DLT645_ERR_PASSWORD_04;
}

//--------------------------------------------------
//功能描述:  645数据格式判断
//         
//参数:      TYPE_PORT PortType[in]
//         
//           WORD Len[in]
//         
//           BYTE *pBuf[in]
//         
//返回值:    WORD
//         
//备注内容:  无
//--------------------------------------------------
static WORD Proc645FrameData( eSERIAL_TYPE PortType )
{
	WORD wResult;
	
	wResult = Judge645AddrCanDo( PortType );
	if( wResult != DLT645_OK_00 )
	{
		return wResult;
	}
	
	wResult = JudgePasswordLevel( PortType );
	if( wResult != DLT645_OK_00 )
	{
		return wResult;
	}
	
	wResult = api_JudgeAuthority(PortType);
	if( wResult != DLT645_OK_00 )
	{
		return wResult;
	}
	
	wResult = JudgeFollowFlag( PortType );
    
	return wResult;
}
//--------------------------------------------------
//功能描述:  645读命令
//         
//参数:      eSERIAL_TYPE PortType[in]
//         
//返回值:
//			DLT645_OK_00 - 正确返回
// 			DLT645_ERR_DATA_01 ~ DLT645_ERR_PRG_KEY_80  -  错误代码
// 			DLT645_NO_RESPONSE - 无需回复
//         
//备注内容:  无
//--------------------------------------------------
WORD ReadData0X11( eSERIAL_TYPE PortType )
{
	DLT645APPFollow[PortType].FollowFlag = eNO_FOLLOW;//如果收到正常读命令则清后续数据标志

	return( GetPro645IDData( PortType, 0xFF, gPr645[PortType].dwDataID.b, MAX_APDU_SIZE, DLT645Sendbuf[PortType].DataBuf ) );
}
//---------------------------------------------------------------
//函数功能: 读取后续帧
//
//参数:    eSERIAL_TYPE PortType[in] - 串口号
//                   
//返回值:  
//			DLT645_OK_00 - 正确返回
// 			DLT645_ERR_DATA_01 ~ DLT645_ERR_PRG_KEY_80  -  错误代码
// 			DLT645_NO_RESPONSE - 无需回复
// 
//备注:   
//---------------------------------------------------------------
WORD ReadContinueData0X12( eSERIAL_TYPE PortType )
{
	if( gPr645[PortType].bSEQ == (DLT645APPFollow[PortType].bLastNum + 1) )
	{
		if( DLT645APPFollow[PortType].FollowFlag == eNORMAL_FOLLOW )
		{
			return DLT645_OK_00;
		}
		else if( DLT645APPFollow[PortType].FollowFlag == eLOAD_FOLLOW )//负荷记录后续帧 需要特殊处理			
		{
			return GetRequest645Lpf( PortType, (gPr645[PortType].pMessageAddr+10), MAX_APDU_SIZE, DLT645Sendbuf[PortType].DataBuf );
		}
	}
    
    return DLT645_OK_00;
}

//--------------------------------------------------
//功能描述:  645-2007协议请求读电能表通信地址，仅支持点对点通信。
//         
//参数:      BYTE *ProBuf[in]
//         		ProBuf: 串口通信缓冲	
//         
//返回值:    0x0000 - 0x7FFF : 数据长度
//			0x8000 + 错误类型
//         
//备注内容:  对应645-2007 中的 如下：
//	7.4.1	主站请求帧
//	功能：请求读电能表通信地址，仅支持点对点通信。
//	地址域：AA…AAH
//	控制码：C=13H
//	数据域长度：L=00H
//7.4.2	从站正常应答帧
//	控制码：C=93H
//　	数据域长度：L=06H
//--------------------------------------------------
WORD DoDlt645_2007ReadCommAddr0X13( eSERIAL_TYPE PortType )
{
	lib_ExchangeData( DLT645Sendbuf[PortType].DataBuf, (BYTE *)&FPara1->MeterSnPara.CommAddr[0], 6 );

	return 6;
}

//--------------------------------------------------
//功能描述:  645-2007协议请求读电能表通信地址，仅支持点对点通信。
//         
//参数:      BYTE *ProBuf[in]
//         		ProBuf: 串口通信缓冲	
//         
//返回值:    无
//         
//备注内容:  1、该命令必须与编程键配合使用；如无编程键，不支持该命令 
//			2、从站异常不应答        
//--------------------------------------------------
WORD DoDlt645_2007WriteCommAddr0X15( eSERIAL_TYPE PortType )
{
	return DLT645_ERR_ID_02;
}

//---------------------------------------------------------------
//函数功能: 需量清零
//
//参数: 	eSERIAL_TYPE PortType
//                   
//			DLT645_OK_00 - 正确返回
// 			DLT645_ERR_DATA_01 - 错误代码
//
//备注:   
//---------------------------------------------------------------
WORD ProClrCurrentMaxDemand( eSERIAL_TYPE PortType )
{
	WORD wResult,wTmpClearMeterType;
	BYTE *ProBuf;
	
	ProBuf = (BYTE *)gPr645[PortType].pMessageAddr;
	
	wResult = DLT645_ERR_DATA_01;
	
	if( api_GetSysStatus( eSYS_STATUS_ID_645AUTH ) == FALSE )
	{		
		return DLT645_ERR_PASSWORD_04;
	}
	
	if( api_GetRunHardFlag( eRUN_HARD_MAC_ERR_FLAG ) == TRUE )
	{ 
		return DLT645_ERR_PASSWORD_04;
	}
	
	#if( MAX_PHASE == 3) //单相表没有需量清零
	if( gPr645[PortType].bLevel == CLASS_2_PASSWORD_LEVEL )
	{
		if( ProBuf[9] < 28 )//16密文4mac+4密码4操作者代码
		{
			return DLT645_ERR_DATA_01;
		}
		//密文
		lib_InverseData( ProBuf + 22, ProBuf[9] - 12 );
		//MAC
		lib_ExchangeData( ProBuf + 22 + 16, ProBuf + 18,  4 );
		
		if( (api_DecipherSecretData( 0x85, ProBuf[9] - 12 + 4, ProBuf + 22, ProBuf )&0x8000) != 0 )
		{
			return DLT645_ERR_PASSWORD_04;
		}
	}
	
	//时间倒序	
	//lib_InverseData( ProBuf+20, 6 );
	
	if( api_JudgeClock645( ProBuf + 2 ) == FALSE )
	{
		return DLT645_ERR_PASSWORD_04;
	}      
	
	wResult = api_SetClearFlag( eCLEAR_DEMAND, 2 );
	if( wResult == FALSE )
	{
		wResult = DLT645_ERR_DATA_01;
	}
	api_SavePrgOperationRecord( ePRG_CLEAR_MD_NO ); //记录需量清零事件
	#endif	//#if( MAX_PHASE == 3)
	return wResult;
}

//---------------------------------------------------------------
//函数功能: 电表清零
//
//参数: 	eSERIAL_TYPE PortType[in]
//			Type
// 				- 0x55 事件清零
//
// 				- 0xAA 电表清零
//
//返回值:
//			DLT645_OK_00 - 正确返回
// 			DLT645_ERR_DATA_01 - 错误代码
//
//备注:   
//---------------------------------------------------------------
WORD ProClrMeter( eSERIAL_TYPE PortType, BYTE Type )
{
	BYTE i;
	BYTE bTmpClearMeterTimes;
	TFourByteUnion CurEnergy[2], EvenID;
	BYTE *ProBuf, File;
	eCLEAR_METER_TYPE ClearFlag;
	eEVENT_INDEX EventOI;
	TFourByteUnion OMD;
	ProBuf = (BYTE *)gPr645[PortType].pMessageAddr;

	#if( PREPAY_MODE == PREPAY_LOCAL )
	if(( api_GetSysStatus( eSYS_STATUS_INSIDE_FACTORY ) == FALSE ) && (Type == 0xAA))
	{	
		return DLT645_ERR_PASSWORD_04;//卡表厂内模式下允许远程清零，厂外不允许清零命令清零
	}
	#endif

	if( (api_GetSysStatus( eSYS_STATUS_ID_645AUTH ) == FALSE)
	 && (api_GetSysStatus( eSYS_STATUS_INSIDE_FACTORY ) == FALSE) )
	{		
		return DLT645_ERR_PASSWORD_04;
	}
	
	if( api_GetRunHardFlag( eRUN_HARD_MAC_ERR_FLAG ) == TRUE )
	{ 
		return DLT645_ERR_PASSWORD_04;
	}
	//私钥下不允许电表清零
	if( (api_GetRunHardFlag( eRUN_HARD_COMMON_KEY ) == FALSE) && (Type == 0xAA) )
	{
		return DLT645_ERR_PASSWORD_04; 
	}
	
	if( Type == 0x55 )
	{
		File = 0x85;
		ClearFlag = eCLEAR_EVENT;
		EventOI = ePRG_CLEAR_EVENT_NO;
	}
	else if( Type == 0xAA )
	{
		File = 0x84; 
		ClearFlag = eCLEAR_METER;
		EventOI = ePRG_CLEAR_METER_NO;
	}
	else
	{
		return DLT645_ERR_DATA_01;
	}
	if( (api_GetSysStatus( eSYS_STATUS_INSIDE_FACTORY ) == TRUE)
		&&(gPr645[PortType].bLevel == MINGWEN_H_PASSWORD_LEVEL) )
	{
		//时间倒序	
		lib_InverseData( ProBuf+20, 6 );
		memcpy( ProBuf + 2, ProBuf + 20, 6 );
	}
	else if( gPr645[PortType].bLevel == CLASS_2_PASSWORD_LEVEL )
	{
		if( ProBuf[9] < 28 )//16密文4mac+4密码4操作者代码
		{
			return DLT645_ERR_DATA_01;
		}
		//密文
		lib_InverseData( ProBuf + 22, ProBuf[9] - 12 );
		//MAC
		lib_ExchangeData( ProBuf + 22 + 16, ProBuf + 18,  4 );
		
		if( (api_DecipherSecretData( File, ProBuf[9] - 12 + 4, ProBuf + 22, ProBuf )&0x8000) != 0 )
		{
			return DLT645_ERR_PASSWORD_04;
		}
	}
	else//其他密码级别返回错误
    {
		return DLT645_ERR_PASSWORD_04;
    }
	
	if( api_JudgeClock645( ProBuf + 2 ) == FALSE )
	{
		return DLT645_ERR_PASSWORD_04;
	}

	//事件清零
	if( Type == 0x55 )
	{
		// memcpy( EvenID.b, ProBuf + 8, 4 );			//此处取事件清零数据标识位置有问题，校园表此处亦有改动；
		memcpy( EvenID.b, ProBuf + 26, 4 );	
		OMD.d = 0x00050043;
		if( EvenID.d != 0xFFFFFFFF )
		{
			return DLT645_ERR_DATA_01;
		}
		api_WritePreProgramData( 1, OMD.d );
	}
	//置位电表清零标志 默认延时1秒，这样实际延时范围500~1500ms,1200bps 连续发送20字节时间为 183ms
	if( api_SetClearFlag( ClearFlag, 2 ) == FALSE )
	{
		return DLT645_ERR_DATA_01;
	}
	api_SavePrgOperationRecord( EventOI ); //记录电表清零事件
	
	return DLT645_OK_00;
}

//---------------------------------------------------------------
//函数功能: 远程费控命令
//
//参数: 	eSERIAL_TYPE PortType
//                   
//返回值:  
//
//备注:   
//---------------------------------------------------------------
WORD ProcRemoteRelayCommand645( eSERIAL_TYPE PortType )
{
	WORD wResult;
	
 	wResult = api_ProcRemoteRelayCommand645( gPr645[PortType].pMessageAddr);// !!!!!!!!gPr645[PortType].byOperatorCode 操作者代码未处理
 	
	//错误返回
	if( wResult & 0x8000 )
	{
		api_WriteFreeEvent(EVENT_RELAY_ERR, wResult);

		if(  gPr645[PortType].bPassword[2] != 0x01 ) //错误回复1个字节
		{
			return DLT645_ERR_PASSWORD_04;
		}
		else //错误回复2个字节
		{
			return wResult;
		}
	}
	else //正确返回
	{
		memcpy( DLT645Sendbuf[PortType].DataBuf, (BYTE *)&wResult, 2 );
			
		return 2;
	}
}

//---------------------------------------------------------------
//函数功能: 广播校时
//
//参数: 	 eSERIAL_TYPE PortType
//                   
//返回值:  
//			DLT645_OK_00 - 正确返回
// 			DLT645_ERR_DATA_01 - 错误代码
//			DLT645_NO_RESPONSE - 无需回复
//
//备注:   
//---------------------------------------------------------------
WORD ReviseTime0X08( eSERIAL_TYPE PortType )
{
	BYTE i;
	DWORD AbsMeterSecTime,AbsSetSecTime;
	TRealTimer TmpRealTimer;
	WORD wResult;
	
	//默认错误
	wResult = DLT645_ERR_DATA_01;
	
	if( api_GetRunHardFlag( eRUN_HARD_ALREADY_BROADCAST_FLAG ) == FALSE )//广播校时一天一次
	{
		api_GetRtcDateTime( DATETIME_20YYMMDDhhmmss, (BYTE *)&TmpRealTimer ); //获取当前时间
		AbsMeterSecTime = api_CalcInTimeRelativeSec((TRealTimer*)&TmpRealTimer);//计算当前时间绝对秒数
	
		//BCD转HEX
		for( i = 0; i < 6; i++ )
		{
			gPr645[PortType].pDataAddr[i] = lib_BBCDToBin( gPr645[PortType].pDataAddr[i] );
		}
		
		//倒序处理
		lib_InverseData( gPr645[PortType].pDataAddr, 6 );
		
		TmpRealTimer.wYear = 2000 + gPr645[PortType].pDataAddr[0];
		memcpy( &TmpRealTimer.Mon, gPr645[PortType].pDataAddr + 1, 5 );
		AbsSetSecTime = api_CalcInTimeRelativeSec((TRealTimer*)&TmpRealTimer);//计算当前时间绝对秒数
	
		//零点前后5分钟内不允许广播校时
		if( ((RealTimer.Hour == 23) && (RealTimer.Min >= 55)) ||((RealTimer.Hour == 0) && (RealTimer.Min < 5 )))
		{
			wResult = DLT645_ERR_DATA_01;
		}
		//结算日前后5分钟不能校时 下发时间与真实时间不超过5分钟
		else if( (JudgeBroadcastMeterTime( 5*60 ) == TRUE) && (labs(AbsMeterSecTime-AbsSetSecTime) < (5*60)) )
		{
			wResult = DLT645_OK_00;
		}
		
		//校时处理
		if( wResult == DLT645_OK_00 )
		{
			if( api_WriteMeterTime(&TmpRealTimer) == TRUE )
			{
				api_SetRunHardFlag( eRUN_HARD_ALREADY_BROADCAST_FLAG );
				wResult = DLT645_OK_00;

				//记录645广播校时自由事件
				api_WriteFreeEvent(EVENT_BROADCAST_WRITE_TIME, 0x0645);
			}
			else
			{
				wResult = DLT645_ERR_DATA_01;
			}
		}
	}

	//判断返回
	if( gPr645[PortType].eAddrType == eTYPE_ADDR_TYPE_99 )
	{
        return DLT645_NO_RESPONSE;
	}
	else
	{
		return wResult;
	}
}
//---------------------------------------------------------------
//函数功能: 多功能段子设置
//
//参数: 	eSERIAL_TYPE PortType
//                   
//返回值: 
//			DLT645_OK_00 - 正确返回
//备注:   
//---------------------------------------------------------------
WORD MultiFunPortCtrl_645( eSERIAL_TYPE PortType )
{
	if (gPr645[PortType].pDataAddr[0]<3)//设置多功能端子：00-时钟秒脉冲，01-需量周期，02-时段投切
	{
	    api_MultiFunPortSet( gPr645[PortType].pDataAddr[0] );
	
		if( gPr645[PortType].eAddrType == eTYPE_ADDR_TYPE_99 )
		{
			return DLT645_NO_RESPONSE;
		}
		else
		{
			memcpy( DLT645Sendbuf[PortType].DataBuf, (BYTE *)&GlobalVariable.g_byMultiFunPortType, 1 );
			return 0x01;
		}
	}
	else
	{
		return DLT645_ERR_DATA_01;
	}
	
}
	
//---------------------------------------------------------------
//函数功能: 处理预付费
//
//参数:      eSERIAL_TYPE PortType[in]
//
//返回值:
//			DLT645_OK_00 - 正确返回
// 			DLT645_ERR_DATA_01 ~ DLT645_ERR_PRG_KEY_80  -  错误代码
// 			DLT645_NO_RESPONSE - 无需回复
//
//备注:   
//---------------------------------------------------------------
WORD DealProcPrePay( eSERIAL_TYPE PortType )
{
	WORD ReturnLen = 0;
	ReturnLen = api_ProcProPrePay(PortType, gPr645[PortType].pMessageAddr, DLT645Sendbuf[PortType].DataBuf );
	
	return ReturnLen;
}

//--------------------------------------------------
//功能描述:  645_07应用层函数
//
//参数:      eSERIAL_TYPE PortType[in]
//
//返回值:
//			DLT645_OK_00 - 正确返回
// 			DLT645_ERR_DATA_01 ~ DLT645_ERR_PRG_KEY_80  -  错误代码
// 			DLT645_NO_RESPONSE - 无需回复
//
//备注内容:  无
//--------------------------------------------------
static WORD DealDLT645_2007( eSERIAL_TYPE PortType )
{
	WORD wResult;

	wResult = DLT645_ERR_ID_02;
	
	if( gPr645[PortType].eProtocolType != ePROTOCOL_DLT645_07 )
	{
		return 0xffff;
	}
	switch ( gPr645[PortType].byReceiveControl )
	{
		case eCONTROL_RECE_03:
			wResult = DealProcPrePay(PortType);
			break;
		case eCONTROL_RECE_08:
			wResult = ReviseTime0X08(PortType);
			break;
		case eCONTROL_RECE_11:
			wResult = ReadData0X11(PortType);
			break;
		case eCONTROL_RECE_12:
			wResult = ReadContinueData0X12(PortType);
			break;
		case eCONTROL_RECE_13:
			wResult = DoDlt645_2007ReadCommAddr0X13(PortType);
			break;	
		case eCONTROL_RECE_14:
			wResult = DealSetDlt645_2007Data0X14(PortType);
			break;
		case eCONTROL_RECE_15:
			wResult = DLT645_ERR_ID_02;//DoDlt645_2007WriteCommAddr0X15(PortType);
			break;
		case eCONTROL_RECE_19://需量清零
			wResult = ProClrCurrentMaxDemand( PortType );
			break;
		case eCONTROL_RECE_1A://电表清零
			wResult = ProClrMeter( PortType, 0xAA );
			break;
		case eCONTROL_RECE_1B://事件清零   仅支持总清
			wResult = ProClrMeter( PortType, 0x55 );
			break;
		case eCONTROL_RECE_1C:
			wResult = ProcRemoteRelayCommand645(PortType);
			break;
		case eCONTROL_RECE_1D:
			if ((gPr645[PortType].dwDataID.d > 0x0F0F0F00) && (gPr645[PortType].dwDataID.d < 0x0F0F0F04)) // 升级命令
			{
				wResult = ProIap(PortType);
			}
			else if ((gPr645[PortType].wDataLen == 1) && (gPr645[PortType].pDataAddr[0] < eMAX_OUTPUT)) // 多功能端子设置
			{
				wResult = MultiFunPortCtrl_645(PortType);
			}
			break;
		default:
			break;
	}
	return wResult;
}
//--------------------------------------------------
//功能描述:  厂内扩展应用层函数
//         
//参数:      TYPE_PORT PortType[in]
//         
//返回值:    
// 			DLT645_ERR_DATA_01 ~ DLT645_ERR_PRG_KEY_80  -  错误代码
// 			DLT645_NO_RESPONSE - 无需回复
// 			其他 - 返回数据长度
//         
//备注内容:  无
//--------------------------------------------------
static WORD DealDLT645_Factory( eSERIAL_TYPE PortType )
{
	WORD wResult;
	       
	wResult = 0;
	if( gPr645[PortType].eProtocolType != ePROTOCOL_DLT645_FACTORY )
	{
		return 0xffff;
	}
	
	switch( gPr645[PortType].byReceiveControl )
	{
		case eCONTROL_RECE_11:			
			wResult = ReadFactoryExtPro( gPr645[PortType].pMessageAddr + 14 );
			if( wResult != 0 )
			{
				memcpy( DLT645Sendbuf[PortType].DataBuf, gPr645[PortType].pMessageAddr + 14, wResult );
			}
			else
			{
				wResult = DLT645_ERR_DATA_01;
			}
			break;
		case eCONTROL_RECE_14:
			wResult = WriteFactoryExtPro( gPr645[PortType].pMessageAddr + 22 );
			if( wResult == 0 )
			{
				wResult = DLT645_ERR_DATA_01;
			}
			else if( wResult > 0x8000 )
			{
				return wResult;//得到失败原因
			}
			else
			{
				DLT645Sendbuf[PortType].DataBuf[0] = 0x00;
				wResult = 1;
			}
			break;
		default:
			break;
	}
	
	return wResult;	
}

//--------------------------------------------------
//功能描述:  645应用层函数
//         
//参数:      TYPE_PORT PortType[in]
//         
//返回值:
// 			DLT645_ERR_DATA_01 ~ DLT645_ERR_PRG_KEY_80  -  错误代码
// 			DLT645_NO_RESPONSE - 无需回复
// 			其他 - 数据长度
//         
//备注内容:  无
//--------------------------------------------------
WORD DLT645APPPro(eSERIAL_TYPE PortType)
{
	WORD wResult;
	
	DLT645Sendbuf[PortType].DataBuf = AllocResponseBuf( PortType, ePROTOCO_645 );
    wResult = Proc645FrameData( PortType );
    //解锁密码已在上层Proc645FrameData中处理，且返回FALSE，不会进入下面判断，无需再处理
	if( (wResult == DLT645_OK_00) && (DLT645APPFollow[PortType].FollowFlag != eNORMAL_FOLLOW) )//数据正常或者是读取后续负荷记录
	{
    	if(gPr645[PortType].eProtocolType == ePROTOCOL_DLT645_07 )
    	{
    		wResult = DealDLT645_2007(PortType);//改下名字 DealDLT645_2007
    	}
    	else if( gPr645[PortType].eProtocolType == ePROTOCOL_DLT645_FACTORY )
    	{
    		wResult = DealDLT645_Factory(PortType);//改下名字 DealDLT645_Factory
    	}
    	else
    	{
    		//暂无其他规约	
    	}
	}

	return wResult;
}

//--------------------------------------------------
//功能描述:  645规约处理主函数
//         
//参数:      TYPE_PORT PortType[in]
//         
//           BYTE *pBuf[in]
//         
//返回值:    WORD
//         
//备注内容:  无
//--------------------------------------------------
void ProMessage_Dlt645( TSerial *p )
{
	WORD wResult;
	BYTE i;
	
	for( i=0; i<MAX_COM_CHANNEL_NUM; i++ )
	{
		if( p == &Serial[i] )
		{
        	Proc645LinkDataRequest((eSERIAL_TYPE)i, Serial[i].ProBuf);    //链路层数据处理
        	wResult = DLT645APPPro((eSERIAL_TYPE)i);                      //应用层数据解析
        	Proc645LinkDataResponse( (eSERIAL_TYPE)i, wResult );          //链路层数据组帧
			break;
		}
	}
}
//-----------------------------------------------
// 功能描述  : 645规约预处理
//
// 参数  :    TSerial *p[in]
//
// 返回值:     BOOL （TRUE-需要做转发或者报文重组处理）
// 备注内容  :  无
//-----------------------------------------------
BOOL Pre_Dlt645(TSerial *p)
{
	BYTE i, jlNo, byGy;
	BOOL bRc = FALSE;
	WORD wLen;

	#if(SEL_F415 == YES)
	for (i = 0; i < MAX_COM_CHANNEL_NUM; i++)
	{
		if (p == &Serial[i])
		{
			if (i == eRS485_I)
			{
				if (IsNeedTransmit(i, PRO_NO_SPECIAL, Serial[i].ProBuf, &jlNo))
				{
					JLWrite(jlNo, Serial[i].ProBuf, (DWORD)((Serial[i].RXWPoint + MAX_PRO_BUF - Serial[i].BeginPosDlt645) % MAX_PRO_BUF), PRO_NO_SPECIAL, i);
					bRc = TRUE;
				}
				break;
			}
			else if(i == ePT_UART1_F415)// ePT_UART1_F415
			{
				if (IsJLing(i, &byGy, &jlNo))
				{
					if (jlNo == eRS485_I)
					{
						Serial[i].RXWPoint = (Serial[i].RXWPoint + MAX_PRO_BUF - Serial[i].BeginPosDlt645) % MAX_PRO_BUF;
						memset((void *)Serial[jlNo].ProBuf, 0xfe, 4);
						memcpy((Serial[jlNo].ProBuf + 4), (void *)Serial[i].ProBuf, Serial[i].RXWPoint);

						//使得返回给上位机的报文中，通信地址与上位机发的地址域一致
						memcpy((Serial[jlNo].ProBuf + 5), (BYTE *)&Address_645[0], 6);

						Serial[jlNo].SendLength = (Serial[i].RXWPoint + 4);
						Serial[jlNo].TXPoint = 0;

						// 重新计算帧校验
						wLen = Serial[jlNo].SendLength ;
						Serial[jlNo].ProBuf[wLen-2] = lib_CheckSum( (BYTE *)&(Serial[jlNo].ProBuf[4]), wLen-6);
						// 执行延时
						//	Serial[jlNo].ReceToSendDelay = (DWORD)RECE_TO_SEND_DELAY;
						SerialMap[jlNo].SCIDisableRcv(SerialMap[jlNo].SCI_Ph_Num);
						SerialMap[jlNo].SCIEnableSend(SerialMap[jlNo].SCI_Ph_Num);
						bRc = TRUE;
					}
				}
				else
				{
					//415内部回复6025
					bRc = GyRxMonitor(i, PRO_NO_SPECIAL);
				}
				break;
			}
		}
	}
	// 2、无需转发时，/在采集器模式下对地址不正确的报文进行拦截
	if (bRc == FALSE)
	{
		if (i == eRS485_I)
		{
			if (IsWorkInCllMode())
			{
				// 2.1 地址不对，不进入接收处理函数
				if (JudgeBroadcastMeterNo(&p->ProBuf[1]) == FALSE)
				{
					if (JudgeRecMeterNo_645(&p->ProBuf[1]) == FALSE)
					{
						bRc = TRUE;
					}
				}
			}
		}
	}
	#endif//#if(SEL_F415 == YES)
	return bRc;
}
#endif//#if( SEL_DLT645_2007 == YES )

