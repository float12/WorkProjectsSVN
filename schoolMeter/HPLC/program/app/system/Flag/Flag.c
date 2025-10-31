//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.8.16
//描述		标志管理文件包括：状态标志，控制标志，任务标志，错误标志
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
//标志变量
static TFlagBytes FlagBytes;
__no_init TNoInitDataFlag NoInitDataFlag;
#if( SEL_DLT645_2007 == YES )
extern const BOOL  Meter645FollowStatusConst;
#endif

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//				函数定义
//-----------------------------------------------
//-----------------------------------------------
//函数功能: 设置系统状态
//
//参数: 
//			StatusNo[in]		系统状态号
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_SetSysStatus(BYTE StatusNo)
{
	BYTE i, j;
	
	ASSERT( StatusNo < eSYS_STATUS_NUM_T, 0 );
	
	i = (StatusNo%eSYS_STATUS_NUM_T) / 8;
	j = StatusNo % 8;

	FlagBytes.SysStatus[i] |= (0x01<<j);
}


//-----------------------------------------------
//函数功能: 清除系统状态
//
//参数: 
//			StatusNo[in]		系统状态号
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_ClrSysStatus(BYTE StatusNo)
{
	BYTE i, j;
	
	ASSERT( StatusNo < eSYS_STATUS_NUM_T, 0 );
	
	i = (StatusNo%eSYS_STATUS_NUM_T) / 8;
	j = StatusNo % 8;

	FlagBytes.SysStatus[i] &= ~(0x01<<j);
}


//-----------------------------------------------
//函数功能: 获取系统状态
//
//参数: 
//			StatusNo[in]		系统状态号
//                    
//返回值:  	TRUE:系统状态置位了 FALSE:系统状态未置位
//
//备注:   
//-----------------------------------------------
BOOL api_GetSysStatus(BYTE StatusNo)
{
	BYTE i, j;
	
	ASSERT( StatusNo < eSYS_STATUS_NUM_T, 0 );

	i = ((StatusNo%eSYS_STATUS_NUM_T)>>3);//相当于除以8
	j = (StatusNo&0x07);//相当于对8求模

	if( (FlagBytes.SysStatus[i] & (0x01<<j)) != 0 )
	{
		// 状态有效
		return TRUE;
	}

	// 状态无效
	return FALSE;
}


//-----------------------------------------------
//函数功能: 设置系统运行固化标志
//
//参数: 
//			FlagNo[in]		系统固化标志号
//                    
//返回值:  	TRUE:设置成功	FALSE:设置失败
//
//备注:   
//-----------------------------------------------
BOOL api_SetRunHardFlag(BYTE FlagNo)
{
	BYTE i, j;
	
	ASSERT( FlagNo < eRUN_HARD_NUM_T, 0 );

	i = (FlagNo%eRUN_HARD_NUM_T) / 8;
	j = FlagNo % 8;

	if( FlagBytes.RunHardFlagSafeRom.Flag[i] & (0x01<<j) )//这位已经置1，避免频繁写EEPROM
	{
		return TRUE;
	}
	FlagBytes.RunHardFlagSafeRom.Flag[i] |= (0x01<<j);

	// 从新计算校验
	lib_CheckSafeMemVerify((BYTE *)&FlagBytes.RunHardFlagSafeRom, sizeof(TRunHardFlagSafeRom), REPAIR_CRC);

	if( api_VWriteSafeMem(  GET_SAFE_SPACE_ADDR(RunHardFlagSafeRom), sizeof(TRunHardFlagSafeRom), (BYTE *)&FlagBytes.RunHardFlagSafeRom ) != TRUE )
	{
		// 记录错误
		api_SetError(ERR_SET_RUN_HARD_FLAG);
		return FALSE;
	}

	return TRUE;
}


//-----------------------------------------------
//函数功能: 清除系统运行固化标志
//
//参数: 
//			FlagNo[in]		系统固化标志号
//                    
//返回值:  	TRUE:清除成功	FALSE:清除失败
//
//备注:   
//-----------------------------------------------
BOOL api_ClrRunHardFlag(BYTE FlagNo)
{
	BYTE i, j;
	
	ASSERT( FlagNo < eRUN_HARD_NUM_T, 0 );

	i = (FlagNo%eRUN_HARD_NUM_T) / 8;
	j = FlagNo % 8;

	if( (FlagBytes.RunHardFlagSafeRom.Flag[i] & (0x01<<j)) == 0 )//这位已经清零，避免频繁写EEPROM
	{
		return TRUE;
	}

	FlagBytes.RunHardFlagSafeRom.Flag[i] &= ~(0x01<<j);

	// 从新计算校验
	lib_CheckSafeMemVerify( (BYTE *)&FlagBytes.RunHardFlagSafeRom, sizeof(TRunHardFlagSafeRom), REPAIR_CRC );

	if( api_VWriteSafeMem(  GET_SAFE_SPACE_ADDR(RunHardFlagSafeRom), sizeof(TRunHardFlagSafeRom), (BYTE *)&FlagBytes.RunHardFlagSafeRom ) != TRUE )
	{
		// 记录错误
		api_SetError(ERR_SET_RUN_HARD_FLAG);
		return FALSE;
	}

	return TRUE;
}


//-----------------------------------------------
//函数功能: 获取系统运行固化标志
//
//参数: 
//			FlagNo[in]		系统固化标志号
//                    
//返回值:  	TRUE:相应固化标志置位了	FALSE:相应固化标志未置位或数据不对
//
//备注:   
//-----------------------------------------------
BOOL api_GetRunHardFlag(BYTE FlagNo)
{
	BYTE i, j;
	
	ASSERT( FlagNo < eRUN_HARD_NUM_T, 0 );

	// 检查内存中标志校验是否正确
	if( lib_CheckSafeMemVerify( (BYTE *)&FlagBytes.RunHardFlagSafeRom, sizeof(TRunHardFlagSafeRom), REPAIR_CRC ) != TRUE )
	{
		// 若不正确则从存储器中读取
		if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR(RunHardFlagSafeRom), sizeof(TRunHardFlagSafeRom), (BYTE *)&FlagBytes.RunHardFlagSafeRom ) != TRUE )
		{
			// 记录错误
			api_SetError(ERR_GET_HARD_FLAG);
			return FALSE;
		}
	}

	i = ((FlagNo%eRUN_HARD_NUM_T)>>3);//相当于除以8
	j = (FlagNo&0x07);//相当于对8求模

	if( FlagBytes.RunHardFlagSafeRom.Flag[i] & (0x01<<j) )
	{
		return TRUE;
	}

	return FALSE;
}


//-----------------------------------------------
//函数功能: 复位系统运行固化标志
//
//参数: 	无
//                    
//返回值:  	TRUE:初始化成功	FALSE:初始化失败
//
//备注:   
//-----------------------------------------------
BOOL api_FactoryInitRunHardFlag(void)
{
	memset(&FlagBytes.RunHardFlagSafeRom, 0, sizeof(TRunHardFlagSafeRom));
	
	#if( SEL_DLT645_2007 == YES )
    if( Meter645FollowStatusConst == TRUE )//645开启后续帧
    {
        api_SetRunHardFlag( eRUN_HARD_645_FOLLOW_STATUS );
    }
    #endif
    
	if( api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(RunHardFlagSafeRom), sizeof(TRunHardFlagSafeRom), (BYTE *)&FlagBytes.RunHardFlagSafeRom ) != TRUE )
	{
		// 记录错误
		api_SetError(ERR_SET_RUN_HARD_FLAG);
		return FALSE;
	}

	return TRUE;
}


//-----------------------------------------------
//函数功能: 上电读出系统运行固化标志
//
//参数: 	无
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
static void RunHardFlagPowerOn(void)
{
	// 读出硬件标志
	if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR(RunHardFlagSafeRom), sizeof(TRunHardFlagSafeRom), (BYTE *)&FlagBytes.RunHardFlagSafeRom ) != TRUE )
	{
		memset( (BYTE *)&FlagBytes.RunHardFlagSafeRom, 0x00, sizeof(TRunHardFlagSafeRom) );
		// 记录错误
		api_SetError(ERR_GET_HARD_FLAG);
	}
}


//-----------------------------------------------
//函数功能: 设置系统规约设置固化标志
//
//参数: 
//			FlagNo[in]		规约设置系统固化标志号
//                    
//返回值:  	TRUE:设置成功	FALSE:设置失败
//
//备注:   
//-----------------------------------------------
BOOL api_SetProHardFlag(BYTE FlagNo)
{
	BYTE i, j;
	
	ASSERT( FlagNo < ePRO_HARD_NUM_T, 0 );

	i = (FlagNo%ePRO_HARD_NUM_T) / 8;
	j = FlagNo % 8;
	
	if( FlagBytes.ProHardFlagSafeRom.Flag[i] & (0x01<<j) )//这位已经置1，避免频繁写EEPROM
	{
		return TRUE;
	}	

	FlagBytes.ProHardFlagSafeRom.Flag[i] |= (0x01<<j);

	// 从新计算校验
	lib_CheckSafeMemVerify((BYTE *)&FlagBytes.ProHardFlagSafeRom, sizeof(TProHardFlagSafeRom), REPAIR_CRC);

	if( api_VWriteSafeMem(  GET_SAFE_SPACE_ADDR(ProHardFlagSafeRom), sizeof(TProHardFlagSafeRom), (BYTE *)&FlagBytes.ProHardFlagSafeRom ) != TRUE )
	{
		// 记录错误
		api_SetError(ERR_SET_PRO_HARD_FLAG);
		return FALSE;
	}

	return TRUE;
}


//-----------------------------------------------
//函数功能: 清除系统规约设置固化标志
//
//参数: 
//			FlagNo[in]		系统规约设置固化标志号
//                    
//返回值:  	TRUE:清除成功	FALSE:清除失败
//
//备注:   
//-----------------------------------------------
BOOL api_ClrProHardFlag(BYTE FlagNo)
{
	BYTE i, j;
	
	ASSERT( FlagNo < ePRO_HARD_NUM_T, 0 );

	i = (FlagNo%ePRO_HARD_NUM_T) / 8;
	j = FlagNo % 8;
	
	if( (FlagBytes.ProHardFlagSafeRom.Flag[i] & (0x01<<j)) == 0 )//这位已经清零，避免频繁写EEPROM
	{
		return TRUE;
	}	
	FlagBytes.ProHardFlagSafeRom.Flag[i] &= ~(0x01<<j);

	// 从新计算校验
	lib_CheckSafeMemVerify( (BYTE *)&FlagBytes.ProHardFlagSafeRom, sizeof(TProHardFlagSafeRom), REPAIR_CRC );

	if( api_VWriteSafeMem(  GET_SAFE_SPACE_ADDR(ProHardFlagSafeRom), sizeof(TProHardFlagSafeRom), (BYTE *)&FlagBytes.ProHardFlagSafeRom ) != TRUE )
	{
		// 记录错误
		api_SetError(ERR_SET_PRO_HARD_FLAG);
		return FALSE;
	}

	return TRUE;
}


//-----------------------------------------------
//函数功能: 获取系统规约设置固化标志
//
//参数: 
//			FlagNo[in]		系统规约设置固化标志号
//                    
//返回值:  	TRUE:相应固化标志置位了	FALSE:相应固化标志未置位或数据不对
//
//备注:   
//-----------------------------------------------
BOOL api_GetProHardFlag(BYTE FlagNo)
{
	BYTE i, j;
	
	ASSERT( FlagNo < ePRO_HARD_NUM_T, 0 );

	// 检查内存中标志校验是否正确
	if( lib_CheckSafeMemVerify( (BYTE *)&FlagBytes.ProHardFlagSafeRom, sizeof(TProHardFlagSafeRom), REPAIR_CRC ) != TRUE )
	{
		// 若不正确则从存储器中读取
		if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR(ProHardFlagSafeRom), sizeof(TProHardFlagSafeRom), (BYTE *)&FlagBytes.ProHardFlagSafeRom ) != TRUE )
		{
			// 记录错误
			api_SetError(ERR_GET_HARD_FLAG);
			return FALSE;
		}
	}

	i = ((FlagNo%ePRO_HARD_NUM_T)>>3);//相当于除以8
	j = (FlagNo&0x07);//相当于对8求模

	if( FlagBytes.ProHardFlagSafeRom.Flag[i] & (0x01<<j) )
	{
		return TRUE;
	}

	return FALSE;
}


//-----------------------------------------------
//函数功能: 复位系统规约设置固化标志
//
//参数: 	无
//                    
//返回值:  	TRUE:初始化成功	FALSE:初始化失败
//
//备注: 此函数是否还需要
//-----------------------------------------------
void api_FactoryInitProHardFlag(void)
{
	if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR(ProHardFlagSafeRom), sizeof(TProHardFlagSafeRom), (BYTE *)&FlagBytes.ProHardFlagSafeRom ) != TRUE )
	{
		memset(&FlagBytes.ProHardFlagSafeRom, 0, sizeof(TProHardFlagSafeRom));
		api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(ProHardFlagSafeRom), sizeof(TProHardFlagSafeRom), (BYTE *)&FlagBytes.ProHardFlagSafeRom );
	}
}


//-----------------------------------------------
//函数功能: 上电读出系统规约设置固化标志
//
//参数: 	无
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
static void ProHardFlagPowerOn(void)
{
	// 读出硬件标志
	if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR(ProHardFlagSafeRom), sizeof(TProHardFlagSafeRom), (BYTE *)&FlagBytes.ProHardFlagSafeRom ) != TRUE )
	{
		memset( (BYTE *)&FlagBytes.ProHardFlagSafeRom, 0x00, sizeof(TProHardFlagSafeRom) );
		// 记录错误
		api_SetError(ERR_GET_HARD_FLAG);
	}
}

//-----------------------------------------------
//函数功能: 获取err信息
//
//参数: 
//			pErr[in]	数据指针

//                    
//返回值:  	BYTE		数据长度 
//
//备注:   
//-----------------------------------------------
BYTE api_GetError( BYTE* pErr )
{
	memcpy( pErr, FlagBytes.ErrByte, (MAX_ERROR/8) );
	return (MAX_ERROR/8);
}


//-----------------------------------------------
//函数功能: 设置指定的错误
//
//参数: 
//			Err[in]		指定的错误标志号
//			BIT15		0  错误信息要写入EEPROM
//           			1  错误信息不写入EEPROM
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_SetError(WORD Err)
{
	WORD i, j;
	WORD Temp;

	Temp = (Err&0x7fff);
	ASSERT( Temp < MAX_ERROR, 0 );
	//错误所在的字节
	i = ((Temp%MAX_ERROR)>>3);
	//错误所在的bit
	j = (Temp&0x0007);
	
	//错误标志如果已经存在，不再置错误标志，防止多次写eeprom
	if( api_CheckError( Temp ) == TRUE )//注意要把bit15去掉后再传
	{
		return;
	}

	FlagBytes.ErrByte[i] |= (0x01<<j);

	if( (Err&0x8000) == 0 )
	{
		api_WriteSysUNMsg( Temp );
	}
}


//-----------------------------------------------
//函数功能: 检查当前是否存在指定的错误
//
//参数: 
//			Err[in]		指定的错误标志号
//                    
//返回值:  	TRUE:存在指定的错误  	FALSE:不存在指定的错误
//
//备注:   
//-----------------------------------------------
BOOL api_CheckError(WORD Err)
{
	WORD i, j;
	
	ASSERT( Err < MAX_ERROR, 0 );
	
	i = ((Err%MAX_ERROR)>>3);
	j = (Err&0x0007);
	
	if( FlagBytes.ErrByte[i] & (0x01<<j) )
	{
		return TRUE;
	}

	return FALSE;
}


//-----------------------------------------------
//函数功能: 清除指定的错误标志号
//
//参数: 
//			Err[in]		指定的错误标志号
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_ClrError(WORD Err)
{
	WORD i, j;

	ASSERT( Err < MAX_ERROR, 0 );
	
	i = ((Err%MAX_ERROR)>>3);
	j = (Err&0x0007);

	FlagBytes.ErrByte[i] &= ~(0x01<<j);
}


//-----------------------------------------------
//函数功能: 给所有任务置同一个标志
//
//参数: 
//			FlagID[in]		标志号
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_SetAllTaskFlag( BYTE FlagID )
{
	ASSERT( FlagID < eFLAG_TIMER_T, 0 );
	
	memset( &FlagBytes.TaskTimeFlag[FlagID%eFLAG_TIMER_T], 0xff, sizeof(FlagBytes.TaskTimeFlag[FlagID]) );
}


//-----------------------------------------------
//函数功能: 置任务标志
//
//参数: 
//			TaskID[in]		任务号
//			FlagID[in]		标志号
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_SetTaskFlag( BYTE TaskID, BYTE FlagID )
{
	ASSERT( TaskID < 8*sizeof(FlagBytes.TaskTimeFlag[0]), 0 );
	ASSERT( FlagID < eFLAG_TIMER_T, 0 );
	
	FlagBytes.TaskTimeFlag[FlagID%eFLAG_TIMER_T] |= ( (DWORD)1<<(TaskID) );
}


//-----------------------------------------------
//函数功能: 获取任务标志
//
//参数: 
//			TaskID[in]		任务号
//			FlagID[in]		标志号
//                    
//返回值:  	TRUE:相应标志置位了  FALSE:相应标志未置位
//
//备注:   
//-----------------------------------------------
BOOL api_GetTaskFlag( BYTE TaskID, BYTE FlagID )
{
	ASSERT( TaskID < 8*sizeof(FlagBytes.TaskTimeFlag[0]), 0 );
	ASSERT( FlagID < eFLAG_TIMER_T, 0 );
	
	if( FlagBytes.TaskTimeFlag[FlagID] & ((DWORD)1<<TaskID) )
	{
		return TRUE;
	}
	
	return FALSE;
}


//-----------------------------------------------
//函数功能: 清除任务标志
//
//参数: 
//			TaskID[in]		任务号
//			FlagID[in]		标志号
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_ClrTaskFlag( BYTE TaskID, BYTE FlagID )
{
	ASSERT( TaskID < 8*sizeof(FlagBytes.TaskTimeFlag[0]), 0 );
	ASSERT( FlagID < eFLAG_TIMER_T, 0 );
	
	DISABLE_CLOCK_INT;//500ms标志位由systick产生， 其他由clock产生
	DISABLE_SYSTICK_INT;
	FlagBytes.TaskTimeFlag[FlagID%eFLAG_TIMER_T] &= ~((DWORD)1<<(TaskID));
	ENABLE_SYSTICK_INT;
	ENABLE_CLOCK_INT;
}


//-----------------------------------------------
//函数功能: 标志任务上电处理
//
//参数: 	无
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_PowerUpFlag( void )
{
	RunHardFlagPowerOn();
	ProHardFlagPowerOn();
	
	//判断上电不清零变量是否可靠标志
	if( NoInitDataFlag.CRC32 != lib_CheckCRC32( (BYTE*)&NoInitDataFlag, sizeof(NoInitDataFlag)-sizeof(DWORD) ) )
	{
		api_SetSysStatus(eSYS_STATUS_NO_INIT_DATA_ERR);
	}
	else
	{
		api_ClrSysStatus(eSYS_STATUS_NO_INIT_DATA_ERR);
	}
}

//-----------------------------------------------
//函数功能: 初始化_noinit_标志
//
//参数: 	无
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_InitNoInitFlag( void )
{
	NoInitDataFlag.Flag = 0x55aa88ee;
	NoInitDataFlag.CRC32 = lib_CheckCRC32( (BYTE*)&NoInitDataFlag, sizeof(NoInitDataFlag)-sizeof(DWORD) );
}

