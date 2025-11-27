//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.8.16
//描述		标志管理文件包括：状态标志，控制标志，任务标志，错误标志
//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------

//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
//标志变量
static TFlagBytes FlagBytes;


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
	
	FlagBytes.TaskTimeFlag[FlagID%eFLAG_TIMER_T] &= ~((DWORD)1<<(TaskID));
}


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
	
	// ASSERT( StatusNo < eSYS_STATUS_NUM_T, 0 );
	
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
	
	// ASSERT( StatusNo < eSYS_STATUS_NUM_T, 0 );
	
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
	
	// ASSERT( StatusNo < eSYS_STATUS_NUM_T, 0 );

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
	// ASSERT( Temp < MAX_ERROR, 0 );
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
	
	// ASSERT( Err < MAX_ERROR, 0 );
	
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

	// ASSERT( Err < MAX_ERROR, 0 );
	
	i = ((Err%MAX_ERROR)>>3);
	j = (Err&0x0007);

	FlagBytes.ErrByte[i] &= ~(0x01<<j);
}
