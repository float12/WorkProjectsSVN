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

// __no_init BYTE 				g_645PassWordErrCounter[2];//密码错误次数03 04 上电不初始化

//认证不通过，只能读出表号、通信地址、备案号、当前日期、当前时间、当前日期时间、当前电能、当前剩余金额、红外认证、查询命令，
const DWORD g_dwHongwaiID[] = {0x04000402,0x04000401,0x04800004,0x04000101,0x04000102,0x0400010c,0x00900200,0x078003FF,0x070003FF};
const DWORD FirstDataID[] = {0x04000108,0x04000109,0x04000306,0x04000307,0x04000402,0x0400040E,0x04001001,0x04001002,0x040502ff,0x040605FF,0x04060AFF,0x04060BFF};//确认一下songchen

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//功能描述  :   初始化645参数
//
//参数  : 	无
//
//返回值:      无
//
//备注内容  :   无
//-----------------------------------------------
// void FactoryInitDLT645(void)
// {
// 	// BYTE i;
// 	memset( g_645PassWordErrCounter, 0x00, sizeof(g_645PassWordErrCounter) );
// }

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
			// else if( api_GetSysStatus( eSYS_STATUS_POWER_ON ) == FALSE )
			// {
			// 	return DLT645_ERR_DATA_01;
			// }
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
	// BYTE i;
	BYTE ControlByte;
	// BYTE *ProBuf;
	// ProBuf = (BYTE *)gPr645[PortType].pMessageAddr;
	ControlByte = gPr645[PortType].byReceiveControl;

	// if( ControlByte == eCONTROL_RECE_03 )
	// {
	// 	if( (PortType == eIR)&&(api_GetSysStatus( eSYS_IR_ALLOW_PRG ) == FALSE) )
	// 	{
    //         for( i=0; i<(sizeof(g_dwHongwaiID)/sizeof(DWORD)); i++ )
	// 		{
	// 			if( gPr645[PortType].dwDataID.d == g_dwHongwaiID[i] )
	// 			{
	// 				return DLT645_OK_00;
	// 			}
	// 		}
	// 		return DLT645_ERR_DATA_01;//就是01没错
	// 	}	
	// }
	// else if( ControlByte == eCONTROL_RECE_08 )//广播校时需要判断红外权限
	// {
	// 	if( (PortType == eIR) && (api_GetSysStatus( eSYS_IR_ALLOW_PRG ) == FALSE) )
	// 	{
	// 		if( gPr645[PortType].eAddrType == eTYPE_ADDR_TYPE_99 )
	// 		{
	// 			return DLT645_NO_RESPONSE;
	// 		}
	// 		else
	// 		{
	// 			return DLT645_ERR_PASSWORD_04;
	// 		}
	// 	}
	// }
	if( ControlByte == eCONTROL_RECE_11 )
	{
	    if( gPr645[PortType].eProtocolType == ePROTOCOL_DLT645_FACTORY )
	    {
            return DLT645_OK_00;
	    }
		// else if( (PortType == eIR)&&(api_GetSysStatus( eSYS_IR_ALLOW_PRG ) == FALSE) )
		// {
        //     for( i=0; i<(sizeof(g_dwHongwaiID)/sizeof(DWORD)); i++ )
		// 	{
		// 		if( gPr645[PortType].dwDataID.d == g_dwHongwaiID[i] )
		// 		{
		// 			return DLT645_OK_00;
		// 		}
		// 	}
	
		// 	if(( gPr645[PortType].dwDataID.b[0] == 0x00 )&&( gPr645[PortType].dwDataID.b[3] == 0x00 ))//读当前电能
		// 	{
		// 		return DLT645_OK_00;
		// 	}
			
		// 	return DLT645_ERR_PASSWORD_04;
		// }	
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
		// else if( gPr645[PortType].bLevel == MINGWEN_L_PASSWORD_LEVEL )//04 级
		// {
		// 	return DLT645_ERR_PASSWORD_04;//不可用来设置
		// }
		// else if( (gPr645[PortType].bLevel == CLASS_2_PASSWORD_LEVEL )||( gPr645[PortType].bLevel == CLASS_1_PASSWORD_LEVEL ) )//98 99 级
		// {
		// 	//身份认证在外面已经判过，不再重复
		// 	//MAC挂起
		// 	if( api_GetRunHardFlag(eRUN_HARD_MAC_ERR_FLAG) == TRUE )
		// 	{
		// 		return DLT645_ERR_PASSWORD_04;
		// 	}
				
		// 	if( gPr645[PortType].bLevel == CLASS_2_PASSWORD_LEVEL)//98级
		// 	{
		// 		if(gPr645[PortType].dwDataID.d == 0x04001503)
		// 		{
		// 			return DLT645_OK_00;
		// 		}
		// 		if( api_JudgeFirstData(gPr645[PortType].dwDataID.d) == TRUE )//如果是一类数据，采用99H级密码设置返回密码错
		// 		{
		// 			return DLT645_ERR_PASSWORD_04;
		// 		}	
		// 	}
		// 	else//99级
		// 	{
		// 		if(gPr645[PortType].dwDataID.d == 0x04001503)
		// 		{
		// 			return DLT645_ERR_PASSWORD_04;
		// 		}
		// 	}
		// }
	}
	// else if( ControlByte == eCONTROL_RECE_1C )
	// {
	// 	//密码为98级
	// 	if(gPr645[PortType].bLevel == CLASS_2_PASSWORD_LEVEL)
	// 	{
	// 		//MAC挂起
	// 		if( api_GetRunHardFlag(eRUN_HARD_MAC_ERR_FLAG) == TRUE )
	// 		{
	// 			return R_ERR_MAC;
	// 		}
	// 	}
	// 	else if( (gPr645[PortType].bLevel==MINGWEN_H_PASSWORD_LEVEL) || (gPr645[PortType].bLevel==MINGWEN_L_PASSWORD_LEVEL) )
	// 	{	
	// 		memmove( ProBuf+22, ProBuf+18, 8 );	
	// 		lib_InverseData( ProBuf+24, 6 );
	// 		if( (ProBuf[22]==0x1b) || (ProBuf[22]==0x1c) || (api_GetSysStatus(eSYS_STATUS_INSIDE_FACTORY)==TRUE) )
	// 		{
	// 			//支持02,04级密码明文合闸操作,不经过红外认证也可以合闸
	// 		}
	// 		else
	// 		{
	// 			return R_ERR_PASSWORD;
	// 		}
	// 	}
	// 	else
	// 	{
	// 		return R_ERR_PASSWORD;
	// 	}
	// }

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
// static BOOL CheckUnlockPassword( eSERIAL_TYPE PortType )
// {	
// 	BYTE i;
// 	TMuchPassword Password;

// 	//解锁密码仅在锁定状态下生效
// 	if(	(api_GetRunHardFlag(eRUN_HARD_H_PASSWORD_ERR_STATUS) == FALSE) &&
// 		(api_GetRunHardFlag(eRUN_HARD_L_PASSWORD_ERR_STATUS) == FALSE) &&
// 		(api_GetRunHardFlag(eRUN_HARD_PASSWORD_ERR_STATUS) == FALSE ))
// 	{
// 		return FALSE;	
// 	}

// 	for( i=0; i<3; i++ )
// 	{
// 		if( gPr645[PortType].bPassword[i] != 0xAB )
// 		{
// 			break;
// 		}
// 	}
	
// 	if( i != 3 ) //密码判断不正确
// 	{
// 		return FALSE;
// 	}
// 	else
// 	{
// 		g_645PassWordErrCounter[0] = 0;	//02级密码错误计数清0
// 		g_645PassWordErrCounter[1] = 0;	//04级密码错误计数清0
// 		api_ClrRunHardFlag(eRUN_HARD_PASSWORD_ERR_STATUS);//清零698密码错误标志
// 		api_ClrRunHardFlag(eRUN_HARD_H_PASSWORD_ERR_STATUS);//清零02级密码错误标志
// 		api_ClrRunHardFlag(eRUN_HARD_L_PASSWORD_ERR_STATUS);//清零04级密码错误标志
// 		return TRUE;
// 	}
// }

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
	// BYTE i, j;
	BYTE i;
	// TMuchPassword Password;

	// if(gPr645[PortType].bLevel == SYSTEM_UNLOCK_PASSWORD_LEVEL)
	// {
	// 	if(CheckUnlockPassword(PortType) == TRUE)
	// 	{
	// 		return TRUE;
	// 	}
	// 	return FALSE;
	// }
	
	//从EEPROM读取密码数据 如果EEPROM数据错误 不判密码 防止EEPROM错误或者未初始化无法打开厂内模式
	// if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR(ParaSafeRom.MuchPassword), sizeof(TMuchPassword),(BYTE *)Password.Password645) != TRUE )
	// {
	// 	return TRUE;
	// }

	for( i=0; i<3; i++ )
	{
		// if( gPr645[PortType].bPassword[i] != Password.Password645[gPr645[PortType].bLevel][i] )
        if( gPr645[PortType].bPassword[i] != 0x00 )
		{
			break;
		}
	}
	
	if( i != 3 ) //密码判断不正确
	{
		// //密码验证错误 累加错误次数
		// if( gPr645[PortType].bLevel == MINGWEN_H_PASSWORD_LEVEL )
		// {
		// 	if( (++g_645PassWordErrCounter[0]) >= MAX_PASSWORD_ERR_TIMES )
		// 	{			
		// 		api_SetRunHardFlag(eRUN_HARD_H_PASSWORD_ERR_STATUS);
		// 	    api_WriteFreeEvent( EVENT_PROGRAMKEY_LOCK, 0 );
		// 	}
		// }
		// else if( gPr645[PortType].bLevel == MINGWEN_L_PASSWORD_LEVEL )
		// {
		// 	if( (++g_645PassWordErrCounter[1]) >= MAX_PASSWORD_ERR_TIMES )
		// 	{
		// 		api_SetRunHardFlag(eRUN_HARD_L_PASSWORD_ERR_STATUS);
		// 	}
		// }
		return FALSE;
	}
	//密码验证正确时，错误计数清零
	// else if( i == 3 )
	// {
	// 	if( gPr645[PortType].bLevel == MINGWEN_H_PASSWORD_LEVEL )
	// 	{
	// 		g_645PassWordErrCounter[0] = 0;
	// 	}
	// 	else if( gPr645[PortType].bLevel == MINGWEN_L_PASSWORD_LEVEL )
	// 	{
	// 		g_645PassWordErrCounter[1] = 0;
	// 	}
	// }

	//若当前为MINGWEN_H_PASSWORD_LEVEL闭锁状态，即使MINGWEN_H_PASSWORD_LEVEL密码验证正确，也要返回FALSE
	// if( (api_GetRunHardFlag(eRUN_HARD_H_PASSWORD_ERR_STATUS) == TRUE) && (gPr645[PortType].bLevel == MINGWEN_H_PASSWORD_LEVEL) )
	// {
	// 	return FALSE;
	// }
	// //若当前为MINGWEN_L_PASSWORD_LEVEL闭锁状态，即使MINGWEN_L_PASSWORD_LEVEL密码验证正确，也要返回FALSE
	// else if( (api_GetRunHardFlag(eRUN_HARD_L_PASSWORD_ERR_STATUS) == TRUE) && (gPr645[PortType].bLevel == MINGWEN_L_PASSWORD_LEVEL) )
	// {
	// 	return FALSE;
	// }
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
        // if( (gPr645[PortType].bLevel == CLASS_1_PASSWORD_LEVEL)//加密密级不进行密码的判断,只判断是否身份认证通过
        // ||(gPr645[PortType].bLevel == CLASS_2_PASSWORD_LEVEL) )
        // {
        //     if( api_GetSysStatus( eSYS_STATUS_ID_645AUTH ) == TRUE )
        //     {
        //         return DLT645_OK_00;
        //     }
        //     else
        //     {
        //        return DLT645_ERR_PASSWORD_04;
        //     }
        // }
        // else
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
	// WORD wResult, i;
	WORD wResult;
	
	DLT645Sendbuf[PortType].DataBuf = AllocResponseBuf( PortType, ePROTOCO_645 );
    wResult = Proc645FrameData( PortType );
    //解锁密码不进行处理，直接返回结果
	if( (wResult == DLT645_OK_00) && (DLT645APPFollow[PortType].FollowFlag != eNORMAL_FOLLOW)
		&& (gPr645[PortType].bLevel != SYSTEM_UNLOCK_PASSWORD_LEVEL) )//数据正常或者是读取后续负荷记录
	{
    	if(gPr645[PortType].eProtocolType == ePROTOCOL_DLT645_07 )
    	{
    		// wResult = DealDLT645_2007(PortType);//改下名字 DealDLT645_2007
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

#endif//#if( SEL_DLT645_2007 == YES )

