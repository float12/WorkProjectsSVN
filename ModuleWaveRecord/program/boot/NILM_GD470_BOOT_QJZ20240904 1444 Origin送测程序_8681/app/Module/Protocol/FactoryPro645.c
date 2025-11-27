//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	张玉猛
//创建日期	2016.9.23
//描述		厂内功能，供规约调用
//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"


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
//				本文件使用的变量，常量
//-----------------------------------------------



//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//				函数定义
//-----------------------------------------------

// static const WORD FactoryOI_Table[] = 
// {
//    0x0201,//厂内模式
//    0x0206,//低级厂内模式
//    0x0205,//需量进位
//    0x0C00,//报警继电器
//    0x0C01,//lcd小铃铛报警控制参数
//    0x0C02,//液晶背光
//    0x0C03,//液晶err报警
// };



//-----------------------------------------------
//函数功能: 扩展规约读函数
//
//参数:		pBuf[in]	0--扩展DI0 1--DI1 2--0XDF 3--0XDB	
//						
//返回值:	返回读取数据长度
//		   
//备注:规约见《电表软件平台扩展规约》:抄读时在04dfdfdb后面扩展4个字节，设置时在数据域扩展8个字节（扩展字节以下简称扩展标识）。
//	第1,2字节为厂内扩展数据标识，分别对应DI0，DI1.
//	第3,4字节固定为0xdf，0xdb。
//	第5,6,7,8字节为当前时间与00年1月1日0时0分的相对分钟数（hex码，传输时高在后，低在前）（抄读时无此数据）。
//-----------------------------------------------
WORD ReadFactoryExtPro(BYTE *pBuf)
{
	// BYTE i,j,k;
	BYTE i;
	BYTE *p;
	// WORD tw,wLen,wReturnLen;
	WORD wReturnLen;
	DWORD td;
	// WORD Result;

	// j = 0;
	// k = 0;
	if( (pBuf[2]!=0xdf) || (pBuf[3]!=0xdb) )
	{
		return 0;
	}
	
	p = pBuf + 2;
	// tw = pBuf[0];
	// wLen = 0;
	//返回数据要添加两个字节的扩展数据标识
	wReturnLen = 0;
		
	switch(pBuf[1])
	{
		// case 0x00://自由事件记录		
		// case 0x01://异常事件记录			
		// 	if(tw == 0)
		// 	{
		// 		return 0;
		// 	}
			
		// 	for(i=1;i<=6;i++)
		// 	{
		// 		if( pBuf[1] == 0x00 )//自由事件
		// 		{
		// 			wLen = api_ReadFreeEvent((tw-1)*6+i,p);
		// 		}
		// 		else
		// 		{
		// 			wLen = api_ReadSysUNMsg((tw-1)*6+i,p);
		// 		}
				
		// 		p += wLen;
		// 		wReturnLen += wLen;
		// 	}
			
		// 	wReturnLen += 2;
		// 	break;
			
		case 0x02:
			if( pBuf[0] == 0x01 )//厂内模式
			{
				if( api_GetSysStatus(eSYS_STATUS_INSIDE_FACTORY) == TRUE )
				{
					pBuf[2] = 0x01;
				}
				else
				{
					pBuf[2] = 0x00;
				}
				
				wReturnLen = 3;
			}
			// else if( pBuf[0] == 0x03 )//自热是否补偿相角 0-不补偿 1-补偿
			// {
			// 	if( api_GetProHardFlag( ePRO_HARD_PHASE_HEAT ) == TRUE )
			// 	{
			// 		pBuf[2] = 0x01;
			// 	}
			// 	else
			// 	{
			// 		pBuf[2] = 0x00;
			// 	}
				
			// 	wReturnLen = 3;
			// }
			// #if( PREPAY_MODE == PREPAY_LOCAL )
			// else if(pBuf[0] == 0x04)
			// {
			// 	//所有阶梯结算日无效情况，且阶梯电价非零情况下，阶梯电价如何扣费：0--不扣费，1-按月阶梯扣费并在每月第1结算日转存阶梯用电量
			// 	if( api_GetProHardFlag( ePRO_HARD_LADDER_SAVE_PARA_INVALID_DEAL ) == TRUE )
			// 	{
			// 		pBuf[2] = 0x01;
			// 	}
			// 	else
			// 	{
			// 		pBuf[2] = 0x00;
			// 	}
				
			// 	wReturnLen = 3;
			// }
			// #endif
			// else if( pBuf[0] == 0x05 ) //需量最低位是否要进位 0—进位 1—不进位（下一位不足0.5的也进位加1）
			// {
			// 	if( api_GetProHardFlag( ePRO_HARD_DEMAND_DATA_CARRY ) == TRUE )
			// 	{
			// 		pBuf[2] = 0x01;
			// 	}
			// 	else
			// 	{
			// 		pBuf[2] = 0x00;
			// 	}

			// 	wReturnLen = 3;
			// }
			break;
		case 0x03://厂内保留存储区
			// if( pBuf[0] == 0x01 )
			// {
			// 	wLen = pBuf[5];
			// 	tw = pBuf[4] + wLen;//pBuf[5];
			// 	if(tw > GET_STRUCT_MEM_LEN(TConMem,FactoryRcvArea))
			// 	{
			// 		break;
			// 	}
			// 	if( api_ReadFromContinueEEPRom( GET_CONTINUE_ADDR( FactoryRcvArea ) + pBuf[4], wLen, p ) == TRUE )
			// 	{
			// 		wReturnLen = wLen + 2;
			// 	}
			// }
			break;
		case 0x04:
			// if(( api_GetSysStatus( eSYS_STATUS_INSIDE_FACTORY ) == FALSE )//未打开厂内模式
			// &&( api_GetSysStatus( eSYS_STATUS_LOW_INSIDE_FACTORY ) == FALSE ))//未打开低级厂内模式
            if( api_GetSysStatus( eSYS_STATUS_INSIDE_FACTORY ) == FALSE )//未打开厂内模式
			{
				break;
			}
			
			memcpy((BYTE*)&td,pBuf+4,4);
			i = pBuf[8];
			
			if( (pBuf[0]==0x00) || (pBuf[0]==0x01) )
			{
				//读内部ram及内部flash
				wReturnLen = api_ReadCpuRamAndInFlash( pBuf[0], td, i, p ) + 2;
			}
			else if(pBuf[0] == 0x02)//读管理芯第一片EEPROM
			{
				wReturnLen = api_ReadSaveMem( 0x01, td, i, p ) + 2;
			}
			else if(pBuf[0] == 0x03)//读管理芯第二片EEPROM
			{
				wReturnLen = api_ReadSaveMem( 0x02, td, i, p ) + 2;
			}
			else if(pBuf[0] == 0x04)//读管理芯FLASH
			{
				wReturnLen = api_ReadSaveMem( 0x11, td, i, p ) + 2;
			}
			else if(pBuf[0]==0x06)
			{
				//读连续空间
				wReturnLen = api_ReadSaveMem( 0x31, td, i, p ) + 2;
			}
			break;
		// case 0x06://读取采样系数
		// 	memcpy((BYTE*)&tw,pBuf+4,2);
		// 	i = pBuf[6];
		// 	if(i > 200)
		// 	{
		// 		return 0;
		// 	}
		// 	if( api_ProcSampleCorr( READ, tw, i, p ) == TRUE )
		// 	{
		// 		wReturnLen = i + 2;
		// 	}
		// 	break;
		// case 0x08://读电表版本
		// 	if(pBuf[0] == 0xdb)
		// 	{
		// 		wReturnLen = api_ReadMeterVersion(p);
		// 		wReturnLen += 2;
		// 	}
		// 	break;
		// #if(RTC_CHIP == RTC_HT_INCLOCK)
		// case 0x09:
		// 	if(pBuf[0] == 0xff)
		// 	{
		// 		wReturnLen = api_ProcRtcPara(READ, p);
		// 		wReturnLen += 2;
		// 	}
		// 	break;
		// #endif
	// 	case 0x0c:
	// 		// 0 -- 报警继电器
	// 		// 1 -- LCD小铃铛报警控制参数
	// 		// 2 -- 液晶背光
	// 		// 3 -- 液晶ERR显示报警
	// 		if(pBuf[0] < 0x04)
	// 		{
	// 			api_ProcLcdCtrlPara(READ,pBuf[0],8,p);
	// 			wReturnLen = 10;
	// 		}
	// 		break;
	// 	case 0x0D://读取大盖、端钮盖状态 0扣盖 1合盖（单相表第二字节端钮盖固定传0）
	// 		if(pBuf[0] == 0x00)
	// 		{
	// 			memset( p, 0x00, 2 );
	// 			if( UP_COVER_IS_OPEN )//上盖开盖
	// 			{
	// 				p[0] = 1;
	// 			}

	// 			#if( MAX_PHASE != 1)
	// 			if( END_COVER_IS_OPEN )//端纽盖开盖
	// 			{
	// 				p[1] = 1;
	// 			}
	// 			#endif
	// 			wReturnLen = 2+2;
	// 		}
	// 		break;
	// 	case 0x0e://读取功率
	// 		if( (pBuf[0]==0x00) || (pBuf[0]==0x01) || (pBuf[0]==0x02) || (pBuf[0]==0x03) )
	// 		{
	// 			Result = api_GetRemoteData(PHASE_ALL+(pBuf[0]*0x1000)+REMOTE_P, DATA_BCD, 6, 4, p );
	// 			if( Result == FALSE )
	// 			{
	// 				return 0;
	// 			}
	// 			else
	// 			{
	// 				wReturnLen = 4+2;
	// 			}
	// 		}
	// 		else if( pBuf[0] == 0xff )
	// 		{
	// 			#if( MAX_PHASE != 1)
	// 			for( i=0; i< 4; i++)
	// 			#else
	// 			for( i=0; i< 2; i++)
	// 			#endif
	// 			{
	// 				Result = api_GetRemoteData(PHASE_ALL+(i*0x1000)+REMOTE_P, DATA_BCD, 6, 4, p );
	// 				if( Result == FALSE )
	// 				{
	// 					return 0;
	// 				}
	// 				p = p+4;
	// 			}
				
	// 			wReturnLen = ( 4*i + 2 );
	// 		}
	// 		break;

	// 	case 0x0f:
	// 		if(pBuf[0] == 0x00)//电源异常检查
	// 		{
	// 			#if(MAX_PHASE == 1)
	// //			text[0] = HT_TMR4->TMRCNT;
	// 			for(i=0;i<240;i++)//约668个时钟（32768）20.4ms 和高低电平状态无关
	// 			{
	// 				if(RELAY_STATUS_OPEN)
	// 				{
	// 					j++;
	// 				}
	// 				else
	// 				{
	// 					k++;
	// 				}
	// 				api_Delay100us(1);
	// 			}
	// //			text[1] = HT_TMR4->TMRCNT;
	// 			p[0] = j;
	// 			p[1] = 0;
	// 			wReturnLen = 2+2;
	// 			#else
	// 			return 0;
	// 			#endif
	// 		}
	// 		else if( pBuf[0] == 0x01 ) //检查掉电保存是否完整
	// 		{
	// 			p[0] = api_GetPowerDownCompleteFlag();
	// 			wReturnLen = 2+1;
	// 		}
	// 		#if( PREPAY_MODE == PREPAY_LOCAL )
	// 		else if( pBuf[0] == 0x02 )//esam版本检测
	// 		{
	// 		   if( api_GetSysStatus( eSYS_STATUS_INSIDE_FACTORY ) == TRUE ) 
	// 		   {
    //                 p[0] = api_TestEsamCard( );
	// 		   }
	// 		   else
	// 		   {
    //                 p[0] = FALSE;
	// 		   }
               
    //            wReturnLen = 2+1;
	// 		}
	// 		#endif
	// 		break;
	// 	case 0x10:
	// 		if(pBuf[0] == 0x00)//表内错误信息字
	// 		{
	// 			memset( p, 0x00, 8 );
	// 			for(i = 0; i < 64; i++)
	// 			{
	// 				if(api_CheckError( i ) == TRUE)
	// 				{
	// 					p[i / 8] |= (0x01 << (i % 8));
	// 				}
	// 			}
	// 			wReturnLen = 2 + 8;
	// 		}
	// 		break;
			
		default:
			break;
	}
	
	return wReturnLen;
}


//-----------------------------------------------
//函数功能: 扩展规约写函数
//
//参数:		pBuf[in]	0--扩展DI0 1--DI1 2--0XDF 3--0XDB	
//						4,5,6,7--相对分钟数
//返回值:	返回值bit15为1为错误，其他为正确
//		   
//备注:规约见《电表软件平台扩展规约》
//-----------------------------------------------
WORD WriteFactoryExtPro(BYTE *pBuf)
{
	DWORD tdw;
	// WORD tw,wReturnLen,OI;
	WORD wReturnLen,OI;
	// BYTE i;
    // BOOL Result;
	
	if( (pBuf[2]!=0xdf) || (pBuf[3]!=0xdb) )
	{
		return 0;
	}
	
	if( api_GetSysStatus( eSYS_STATUS_INSIDE_FACTORY ) == FALSE )//未打开厂内模式
	{
		memcpy( (BYTE*)&OI, pBuf, sizeof(WORD) );
		
		// if( api_GetSysStatus( eSYS_STATUS_LOW_INSIDE_FACTORY ) == TRUE )//打开低级厂内模式
		// {
		// 	if( (OI == 0x0201 )//厂内模式
		// 	|| (OI == 0x0206 )//低级厂内模式
		// 	|| (OI == 0x0205 )//需量进位
		// 	|| (OI == 0x0C00 )//报警继电器
		// 	|| (OI == 0x0C01 )//lcd小铃铛报警控制参数
		// 	|| (OI == 0x0C02 )//液晶背光
		// 	|| (OI == 0x0C03 )//液晶err报警
		// 	)
		// 	{
		// 		//可以执行
		// 	}
		// 	else
		// 	{
		// 		return 0x8001;
		// 	}
		// }
		// else
		{
			if( (OI != 0x0201 ) && (OI != 0x0206 ))//只允许打开厂内模式和低级厂内模式
			{
				return 0x8001;
			}
		}
	}
	
	//判断时标
	// memcpy( (BYTE*)&tdw, pBuf+4, 4 );
	// if( api_GetRunHardFlag(eRUN_HARD_ERR_RTC_FLAG) == TRUE )
	// {
	// 	//时间错误，不判断时标	
	// }
	// else
	// {
	// 	if( ((api_GetRunHardFlag(eRUN_HARD_COMMON_KEY)==TRUE) || (UP_COVER_IS_OPEN)) || (FactoryTime != 0) )
	// 	{
	// 		//公钥判断全8时标或者在（当前时间~当前时间+10080）窗口内
	// 		if(tdw != 0x88888888L)//0x054c5638
	// 		{
	// 			if( (g_RelativeMin>tdw) || (tdw>(g_RelativeMin+10080)) )//1440*7=10080
	// 			{
	// 				return 0x8001;
	// 			}			
	// 		}
	// 	}
	// 	else
	// 	{
	// 		//私钥判断（当前时间~当前时间+10080）窗口内
	// 		if( (g_RelativeMin>tdw) || (tdw>(g_RelativeMin+10080)) )//1440*7=10080
	// 		{
	// 			return 0x8001;
	// 		}
	// 	}
	// }
	
	
	wReturnLen = 0;
	
	switch(pBuf[1])
	{
		case 0x02:
			if(pBuf[0] == 0x01)
			{
				//厂内模式
				if(pBuf[8] == 0x01)
				{
					//开盖 或 公钥 或 厂内时间不为零
					// if( (UP_COVER_IS_OPEN) 
					// 	|| (api_GetRunHardFlag(eRUN_HARD_COMMON_KEY)==TRUE) 
					// 	|| (FactoryTime != 0) )
					{
						api_SetSysStatus(eSYS_STATUS_INSIDE_FACTORY);
						
						// api_WriteFreeEvent( EVENT_OPEN_FAC_MODE, FactoryTime );
						wReturnLen = 1;
					}
				}
				else
				{
					api_ClrSysStatus(eSYS_STATUS_INSIDE_FACTORY);
					wReturnLen = 1;
				}
			}
			// else if(pBuf[0] == 0x03)
			// {
			// 	//自热是否补偿相角 0-不补偿 1-补偿
			// 	if(pBuf[8] == 0x01)
			// 	{
			// 		if( api_SetProHardFlag( ePRO_HARD_PHASE_HEAT ) != TRUE )
			// 		{
			// 			break;
			// 		}
			// 		wReturnLen = 1;
			// 	}
			// 	else
			// 	{
			// 		if( api_ClrProHardFlag( ePRO_HARD_PHASE_HEAT ) != TRUE )
			// 		{
			// 			break;
			// 		}
			// 		wReturnLen = 1;
			// 	}
			// }
			// #if( PREPAY_MODE == PREPAY_LOCAL )
			// else if(pBuf[0] == 0x04)
			// {
			// 	//所有阶梯结算日无效情况，且阶梯电价非零情况下，阶梯电价如何扣费：0--不扣费，1-按月阶梯扣费并在每月第1结算日转存阶梯用电量
			// 	if(pBuf[8] == 0x01)
			// 	{
			// 		if( api_SetProHardFlag( ePRO_HARD_LADDER_SAVE_PARA_INVALID_DEAL ) != TRUE )
			// 		{
			// 			break;
			// 		}
			// 		wReturnLen = 1;
			// 	}
			// 	else
			// 	{
			// 		if( api_ClrProHardFlag( ePRO_HARD_LADDER_SAVE_PARA_INVALID_DEAL ) != TRUE )
			// 		{
			// 			break;
			// 		}
			// 		wReturnLen = 1;
			// 	}
			// }
			// #endif
			// else if( pBuf[0] == 0x05 )
			// {
			// 	//需量最低位是否要进位 0—进位 1—不进位（下一位不足0.5的也进位加1）
			// 	if( pBuf[8] == 0x01 )
			// 	{
			// 		if( api_SetProHardFlag( ePRO_HARD_DEMAND_DATA_CARRY ) != TRUE )
			// 		{
			// 			break;
			// 		}
			// 		wReturnLen = 1;
			// 	}
			// 	else
			// 	{
			// 		if( api_ClrProHardFlag( ePRO_HARD_DEMAND_DATA_CARRY ) != TRUE )
			// 		{
			// 			break;
			// 		}
			// 		wReturnLen = 1;
			// 	}
			// }
			// else if( pBuf[0] == 0x06 )
			// {
			// 	//低等级厂内模式-延时时间30min
			// 	if( pBuf[8] == 0x01 )
			// 	{
			// 		api_SetSysStatus( eSYS_STATUS_LOW_INSIDE_FACTORY );
			// 		LowFactoryTime = (30*60);//30分钟时效
			// 		wReturnLen = 1;
			// 	}
			// 	else
			// 	{
			// 		api_ClrSysStatus( eSYS_STATUS_LOW_INSIDE_FACTORY );
			// 		LowFactoryTime = 0;
			// 		wReturnLen = 1;
			// 	}
			// }
			else
			{}
			break;
		// case 0x03://写厂内保留区
		// 	if(pBuf[0] != 0x01)
		// 	{
		// 		break;
		// 	}
		// 	//偏移+长度
		// 	tw = pBuf[8] + pBuf[9];
		// 	if(tw > GET_STRUCT_MEM_LEN(TConMem,FactoryRcvArea) )
		// 	{
		// 		break;
		// 	}
		// 	if( api_WriteToContinueEEPRom( GET_CONTINUE_ADDR( FactoryRcvArea ) + pBuf[8], pBuf[9], pBuf + 10 ) != TRUE )
		// 	{
		// 		break;
		// 	}
		// 	wReturnLen = 1;
		// 	break;
		// case 0x06:
		// 	if(pBuf[0] == 0x01)//写修正系数等
		// 	{
		// 		memcpy( (BYTE*)&tw, pBuf+8, 2 );
		// 		if( api_ProcSampleCorr( WRITE, tw, pBuf[10], pBuf + 12 ) != TRUE )
		// 		{
		// 			break;
		// 		}
		// 		wReturnLen = 1;
		// 	}
		// 	else if(pBuf[0] == 0x02)//校表
		// 	{
		// 		//AA  HF常数  AA  A相瞬时量  AA  B相瞬时量  AA  C相瞬时量  AA
		// 		if( api_ProcSampleAdjust( pBuf[9], pBuf + 10 ) != TRUE )
		// 		{
		// 			break;
		// 		}
		// 		wReturnLen = 1;
		// 	}
		// 	break;
		// case 0x07:
		// 	if(pBuf[0] == 0x00)
		// 	{
		// 		//电表复位操作
		// 		wReturnLen = 1;
		// 		api_PowerDownEnergy();         // 掉电电能转存处理
		// 		Reset_CPU();				
		// 	}
		// 	else if(pBuf[0] == 0xdb)
		// 	{
		// 		//电表清零及电表初始化
		// 		//ClearMeter();			//0X1111
		// 		//api_FactoryInitTask();	//0X2222
		// 		api_SetClearFlag( eCLEAR_FACTORY, 2 );
		// 		wReturnLen = 1;
		// 	}
		// 	else if( pBuf[0] == 0xdf )
		// 	{
		// 		api_SetClearFlag( eCLEAR_PARA, 2 );
		// 		wReturnLen = 1;
		// 	}
		// 	break;
		// #if(RTC_CHIP == RTC_HT_INCLOCK)	
		// case 0x09:
		// 	if(pBuf[0] == 0x00)//RTC常温调校值
		// 	{
		// 		memcpy( (BYTE*)&tw, pBuf+8, 2 );
		// 		if( api_CaliRtcError( tw ) != TRUE )
		// 		{
		// 			break;
		// 		}
		// 		wReturnLen = 1;
		// 	}
		// 	else if(pBuf[0] == 0xff)//写高低温调校系数
		// 	{
		// 		api_ProcRtcPara(WRITE,pBuf + 10);
		// 		wReturnLen = 1;
		// 	}
		// 	break;
		// #endif
		case 0x0a://程序升级
			// if(pBuf[0] == 0x00)//开始接收升级报文
			// {
			// 	memcpy((BYTE*)&tdw,pBuf+8+6,4);
			// 	if(tdw != 0xdfdb0001)
			// 	{
			// 		break;
			// 	}
				
			// 	api_WriteUpdateProgHead(pBuf+8);
				
			// 	wReturnLen = 1;
			// }
			// else if(pBuf[0] == 0x01)//接收升级数据
			// {
			// 	Result = api_WriteUpdateProgMessage(pBuf+8);
			// 	if(Result != TRUE)
			// 	{
			// 		break;
			// 	}
			// 	wReturnLen = 1;
			// }
			if(pBuf[0] == 0x02)//启动电表程序升级
			{
				memcpy((BYTE*)&tdw,pBuf+8+6,4);
				if(tdw != 0xa55a5aa5)
				{
					break;
				}
				
				//此函数不会有返回，在里面有个复位操作
				api_StartUpdateProgMessage(pBuf+8);
				
				wReturnLen = 1;
			}
			break;
		// case 0x0c:
		// 	// 0 -- 报警继电器
		// 	// 1 -- LCD小铃铛报警控制参数
		// 	// 2 -- 液晶背光
		// 	// 3 -- 液晶ERR显示报警
		// 	if(pBuf[0] < 0x04)
		// 	{
		// 		api_ProcLcdCtrlPara(WRITE,pBuf[0],8,pBuf+8);
        //         wReturnLen = 1;
		// 	}
		// 	break;
		// case 0x0D:
		// 	//校准电池电压
		// 	if(pBuf[0] == 0x01)
		// 	{
		// 		if( api_AdjustBatteryVoltage() != TRUE )
		// 		{
		// 			break;
		// 		}

		// 		wReturnLen = 1;
		// 	}
		// 	break;	
		default:
			break;
	}
	
	return wReturnLen;
}




