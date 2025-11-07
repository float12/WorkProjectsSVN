//----------------------------------------------------------------------
//Copyright (C) 2003-20XX 烟台东方威思顿电气有限公司电表软件研发部
//创建人	
//创建日期	
//描述		
//修改记录
//----------------------------------------------------------------------
#include "wsd_def.h"
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
BYTE		SysUNMsgCounter;		//一次上电异常事件记录记录最大次数

#define CONTINUE_SPACE_START_ADDR		0
#define GET_CONTINUE_ADDR( Addr_Name )		( (DWORD)&(((TConMem *)CONTINUE_SPACE_START_ADDR)->Addr_Name) )
//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------
TSysAbrEventConRom SysAbrEventConRom;
//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------

//-----------------------------------------------
//				本文件使用的变量，常量		
//-----------------------------------------------
const char event_name[64] = {"/sysevent.txt"};

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//				函数定义
//-----------------------------------------------
//--------------------------------------------------
//功能描述:  操作事件表到文件系统(此函数只用来记录事件)
//         
//参数:      Operation 1写操作 0 读操作
//           addr 要操作的文件地址
//返回值:    
//         
//备注:  
//--------------------------------------------------
BOOL  api_OperateEventTable( BYTE Operation,int addr,BYTE*Buf,DWORD bLen)//此函数测试没问题
{
	int fd;

	fd = nwy_sdk_fopen(event_name, NWY_RDWR);
	if (fd >= 0)
	{
		if(nwy_sdk_fseek(fd,addr,NWY_SEEK_SET) == addr)
		{
			if (Operation == WRITE)//写操作
			{
				if(nwy_sdk_fwrite(fd,Buf,bLen) == bLen )
				{
					nwy_sdk_fclose(fd);
					nwy_ext_echo("\r\n write file success!!!");
					return TRUE;
				}
				else
				{
					nwy_sdk_fclose(fd);
					return FALSE;
				}
			}
			else
			{
				if (nwy_sdk_fread(fd,Buf,bLen) == bLen)
				{
					nwy_sdk_fclose(fd);
					nwy_ext_echo("\r\n read file success!!!");
					return TRUE;
				}
				else
				{
					nwy_sdk_fclose(fd);
					return FALSE;
				}
			}
		}
		else
		{
			nwy_sdk_fclose(fd);
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}
//-----------------------------------------------
//函数功能: 记录异常事件
//
//参数: 
//			Msg[in]		异常事件类型
//                    
//返回值:  	无
//
//备注: 
//-----------------------------------------------
void api_WriteSysUNMsg(WORD Msg)
{
	DWORD dwAddr;
	WORD Point;

	//在堆栈中开1个结构的空间
	TSysUNMsg TempBuf;
	
	//一次上电最多记录120次
	if( SysUNMsgCounter < (MAX_SYS_UN_MSG-8) )
	{
		SysUNMsgCounter++;
	}
	else
	{
		return;
	}

	//读指针
	api_OperateEventTable(READ,GET_CONTINUE_ADDR( SysAbrEventConRom ),(BYTE *)&Point,sizeof(WORD));

	if( Point >= MAX_SYS_UN_MSG )
	{
		Point = 0;
	}

	TempBuf.EventType = Msg;
	//连续的6个字节填充成年：月：日：时：分：秒（5：4：3：2：1：0）
	api_GetRtcDateTime( DATETIME_YYMMDDhhmmss|DATETIME_SECOND_FIRST|DATETIME_BCD, (BYTE *)&TempBuf.EventTime[0] );
	dwAddr = GET_CONTINUE_ADDR( SysAbrEventConRom ) + sizeof(WORD) + Point*(sizeof(TSysUNMsg));
	api_OperateEventTable(WRITE,dwAddr,(BYTE *)&(TempBuf),sizeof(TSysUNMsg));

	Point ++;
	api_OperateEventTable(WRITE,GET_CONTINUE_ADDR( SysAbrEventConRom ), (BYTE *)&Point, sizeof(WORD));
}


//-----------------------------------------------
//函数功能: 读异常事件
//
//参数: 
//			Num[in]		上Num次异常事件记录
//          Buf[out]	输出缓冲	          
//返回值:  	返回数据长度
//
//备注:   
//-----------------------------------------------
WORD api_ReadSysUNMsg(WORD Num,BYTE *Buf)
{
	WORD ReadPoint;
	DWORD dwAddr;
	
	if( (Num==0) || (Num>MAX_SYS_UN_MSG) )
	{
		return 0;
	}
	
	//读指针
	api_OperateEventTable(READ,GET_CONTINUE_ADDR( SysAbrEventConRom ),(BYTE *)&ReadPoint,sizeof(WORD));

	if( ReadPoint >= Num )
	{
		ReadPoint = ReadPoint - Num;
	}
	else
	{
		ReadPoint = ReadPoint+MAX_SYS_UN_MSG-Num;
	}
	
	dwAddr = GET_CONTINUE_ADDR( SysAbrEventConRom ) + sizeof(WORD) + ReadPoint*(sizeof(TSysUNMsg));
	api_OperateEventTable(READ,dwAddr,Buf,sizeof(TSysUNMsg));
	
	return sizeof(TSysUNMsg);
}
//--------------------------------------------------
//功能描述:  上电创建或打开时间记录文件//必须在上电校时之后
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  api_PowerOnCreatSyseventTable( void )
{
    int fd;

	fd = nwy_sdk_fopen(event_name, NWY_RDWR);
	if(fd < 0)
	{
		fd = nwy_sdk_fopen(event_name, NWY_CREAT | NWY_RDWR);
    	if (fd >= 0)
    	{
			api_OperateEventTable(WRITE,0,(BYTE*)&SysAbrEventConRom,sizeof(SysAbrEventConRom));
    	}
	}
	else
	{
		nwy_sdk_fclose(fd);
	}
}
